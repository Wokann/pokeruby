#include "global.h"
#include "gba/gba.h"
#include "gba/flash_internal.h"
#include "save.h"
#include "load_save.h"
#include "overworld.h"
#include "pokemon.h"
#include "save_failed_screen.h"
#include "ewram.h"

// headless save section?
struct UnkSaveSection
{
    u8 data[0xFF4];
    u32 signature;
}; // size is 0xFF8

static u8 HandleWriteSector(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 HandleWriteSectorNBytes(u8 sectorId, u8 *data, u16 size);
static u8 TryWriteSector(u8 sector, u8 *data);
static u32 RestoreSaveBackupVarsAndIncrement(const struct SaveSectorLocation *locations);
static u32 RestoreSaveBackupVars(const struct SaveSectorLocation *locations);
static u8 HandleWriteIncrementalSector(u16 numSectors, const struct SaveSectorLocation *locations);
static u8 HandleReplaceSectorAndVerify(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 HandleReplaceSector(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 CopySectorSignatureByte(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 WriteSectorSignatureByte(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 TryLoadSaveSlot(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 CopySaveSlotData(u16 sectorId, const struct SaveSectorLocation *locations);
static u8 GetSaveValidStatus(const struct SaveSectorLocation *locations);
static u8 TryLoadSaveSector(u8 sectorId, u8 *data, u16 size);
static bool8 ReadFlashSector(u8 sectorId, struct SaveSector *sector);
static u16 CalculateChecksum(void *data, u16 size);
bool8 unref_sub_8125F4C(struct UnkSaveSection *a1);
u8 unref_sub_8125FA0(void);
u8 unref_sub_8125FF0(u8 *data, u16 size);
u8 unref_sub_8126068(u8 sector, u8 *data, u32 size);
u8 unref_sub_8126080(u8 sector, u8 *data);

// Sector num to begin writing save data. Sectors are rotated each time the game is saved. (possibly to avoid wear on flash memory?)
u16 gLastWrittenSector;
u32 gLastSaveCounter;
u16 gLastKnownGoodSector;
u32 gDamagedSaveSectors;
u32 gSaveCounter;
struct SaveSector *gReadWriteSector; // the pointer is in fast IWRAM but may sometimes point to the slower EWRAM.
u16 gIncrementalSectorId;
u16 gSaveFileStatus;
u32 gGameContinueCallback;

static EWRAM_DATA u32 gLastSaveSectorStatus = 0; // used but in an unferenced function, so unused

/*
 * Sector Layout:
 * 
 * Sectors 0 - 13:      Save Slot 1
 * Sectors 14 - 27:     Save Slot 2
 * Sectors 28 - 29:     Hall of Fame
 * Sectors 30 - 31:     e-Reader battle tower data, maybe?
 * 
 * There are two save slots for saving the player's game data. We alternate between
 * them each time the game is saved, so that if the current save slot is corrupt,
 * we can load the previous one. We also rotate the sectors in each save slot
 * so that the same data is not always being written to the same sector. This
 * might be done to reduce wear on the flash memory, but I'm not sure, since all
 * 14 sectors get written anyway.
 */

// Divide save blocks into individual chunks to be written to flash sectors

#define SAVEBLOCK_CHUNK_EX(structure, size, chunkNum)         \
{                                                             \
    (u8 *)structure + chunkNum * SECTOR_DATA_SIZE,            \
    min(size - chunkNum * SECTOR_DATA_SIZE, SECTOR_DATA_SIZE) \
}                                                             \

#define SAVEBLOCK_CHUNK(structure, chunkNum) SAVEBLOCK_CHUNK_EX(&structure, sizeof(structure), chunkNum)

static const struct SaveSectorLocation sSaveBlockChunks[] =
{
    SAVEBLOCK_CHUNK(gSaveBlock2, 0),

    SAVEBLOCK_CHUNK(gSaveBlock1, 0),
    SAVEBLOCK_CHUNK(gSaveBlock1, 1),
    SAVEBLOCK_CHUNK(gSaveBlock1, 2),
    SAVEBLOCK_CHUNK(gSaveBlock1, 3),

    SAVEBLOCK_CHUNK(gPokemonStorage, 0),
    SAVEBLOCK_CHUNK(gPokemonStorage, 1),
    SAVEBLOCK_CHUNK(gPokemonStorage, 2),
    SAVEBLOCK_CHUNK(gPokemonStorage, 3),
    SAVEBLOCK_CHUNK(gPokemonStorage, 4),
    SAVEBLOCK_CHUNK(gPokemonStorage, 5),
    SAVEBLOCK_CHUNK(gPokemonStorage, 6),
    SAVEBLOCK_CHUNK(gPokemonStorage, 7),
    SAVEBLOCK_CHUNK(gPokemonStorage, 8),
};

static const struct SaveSectorLocation sHallOfFameChunks[] =
{
    SAVEBLOCK_CHUNK_EX(gDecompressionBuffer, 2 * SECTOR_DATA_SIZE, 0),
    SAVEBLOCK_CHUNK_EX(gDecompressionBuffer, 2 * SECTOR_DATA_SIZE, 1),
};

void ClearSaveData(void)
{
    u16 i;

    for (i = 0; i < SECTORS_COUNT; i++)
        EraseFlashSector(i);
}

void Save_ResetSaveCounters(void)
{
    gSaveCounter = 0;
    gLastWrittenSector = 0;
    gDamagedSaveSectors = 0;
}

enum
{
    SECTOR_DAMAGED,
    SECTOR_OK,
    SECTOR_CHECK, // unused
};

static bool32 SetDamagedSectorBits(u8 op, u8 sectorId)
{
    bool32 retVal = FALSE;

    switch (op)
    {
    case SECTOR_DAMAGED:
        gDamagedSaveSectors |= (1 << sectorId);
        break;
    case SECTOR_OK:
        gDamagedSaveSectors &= ~(1 << sectorId);
        break;
    case SECTOR_CHECK: // unused
        if (gDamagedSaveSectors & (1 << sectorId))
            retVal = TRUE;
        break;
    }

    return retVal;
}

// If sectorId is 0xFFFF, this function will write all of the sectors pointed to by 'locations'.
// Otherwise, it will write a single sector with the given 'sectorId'.
static u8 WriteSaveSectorOrSlot(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u32 status;
    u16 i;

    gReadWriteSector = gSaveDataBuffer;

    if (sectorId != 0xFFFF)  // write single chunk
    {
        status = HandleWriteSector(sectorId, locations);
    }
    else  // write all chunks
    {
        gLastKnownGoodSector = gLastWrittenSector;
        gLastSaveCounter = gSaveCounter;
        gLastWrittenSector++;
        gLastWrittenSector %= NUM_SECTORS_PER_SLOT;
        gSaveCounter++;
        status = SAVE_STATUS_OK;

        for (i = 0; i < NUM_SECTORS_PER_SLOT; i++)
            HandleWriteSector(i, locations);

        // Check for any bad sectors
        if (gDamagedSaveSectors != 0) // skip the damaged sector.
        {
            status = SAVE_STATUS_ERROR;
            gLastWrittenSector = gLastKnownGoodSector;
            gSaveCounter = gLastSaveCounter;
        }
    }

    return status;
}

static u8 HandleWriteSector(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u16 i;
    u16 sector;
    u8 *data;
    u16 size;

    // select sector number
    sector = sectorId + gLastWrittenSector;
    sector %= NUM_SECTORS_PER_SLOT;
    // select save slot
    sector += NUM_SECTORS_PER_SLOT * (gSaveCounter % 2);

    data = locations[sectorId].data;
    size = locations[sectorId].size;

    // clear save section.
    for (i = 0; i < sizeof(struct SaveSector); i++)
        ((u8 *)gReadWriteSector)[i] = 0;

    gReadWriteSector->id = sectorId;
    gReadWriteSector->signature = SECTOR_SIGNATURE;
    gReadWriteSector->counter = gSaveCounter;
    for (i = 0; i < size; i++)
        gReadWriteSector->data[i] = data[i];
    gReadWriteSector->checksum = CalculateChecksum(data, size);

    return TryWriteSector(sector, gReadWriteSector->data);
}

static u8 HandleWriteSectorNBytes(u8 sectorId, u8 *data, u16 size)
{
    u16 i;
    struct SaveSector *sector = gSaveDataBuffer;

    for (i = 0; i < sizeof(struct SaveSector); i++)
        ((char *)sector)[i] = 0;

    sector->signature = SECTOR_SIGNATURE;
    for (i = 0; i < size; i++)
        sector->data[i] = data[i];
    sector->id = CalculateChecksum(data, size); // though this appears to be incorrect, it might be some sector checksum instead of a whole save checksum and only appears to be relevent to HOF data, if used.

    return TryWriteSector(sectorId, sector->data);
}

static u8 TryWriteSector(u8 sector, u8 *data)
{
    if (ProgramFlashSectorAndVerify(sector, data) != 0) // is damaged?
    {
        SetDamagedSectorBits(SECTOR_DAMAGED, sector); // set damaged sector bits.
        return SAVE_STATUS_ERROR;
    }
    else
    {
        SetDamagedSectorBits(SECTOR_OK, sector); // unset damaged sector bits. it's safe now.
        return SAVE_STATUS_OK;
    }
}

static u32 RestoreSaveBackupVarsAndIncrement(const struct SaveSectorLocation *locations) // locations are unused
{
    gReadWriteSector = gSaveDataBuffer;
    gLastKnownGoodSector = gLastWrittenSector;
    gLastSaveCounter = gSaveCounter;
    gLastWrittenSector++;
    gLastWrittenSector %= NUM_SECTORS_PER_SLOT;
    gSaveCounter++;
    gIncrementalSectorId = 0;
    gDamagedSaveSectors = 0;
    return 0;
}

static u32 RestoreSaveBackupVars(const struct SaveSectorLocation *locations) // locations are unused
{
    gReadWriteSector = gSaveDataBuffer;
    gLastKnownGoodSector = gLastWrittenSector;
    gLastSaveCounter = gSaveCounter;
    gIncrementalSectorId = 0;
    gDamagedSaveSectors = 0;
    return 0;
}

static u8 HandleWriteIncrementalSector(u16 numSectors, const struct SaveSectorLocation *locations)
{
    u8 status;

    if (gIncrementalSectorId < numSectors - 1)
    {
        status = SAVE_STATUS_OK;
        HandleWriteSector(gIncrementalSectorId, locations);
        gIncrementalSectorId++;
        if (gDamagedSaveSectors)
        {
            status = SAVE_STATUS_ERROR;
            gLastWrittenSector = gLastKnownGoodSector;
            gSaveCounter = gLastSaveCounter;
        }
    }
    else
    {
        status = SAVE_STATUS_ERROR;
    }

    return status;
}

static u8 HandleReplaceSectorAndVerify(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u8 status = SAVE_STATUS_OK;

    HandleReplaceSector(sectorId - 1, locations);

    if (gDamagedSaveSectors)
    {
        status = SAVE_STATUS_ERROR;
        gLastWrittenSector = gLastKnownGoodSector;
        gSaveCounter = gLastSaveCounter;
    }
    return status;
}

static u8 HandleReplaceSector(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u16 i;
    u16 sector;
    u8 *data;
    u16 size;
    u8 status;

    // select sector number
    sector = sectorId + gLastWrittenSector;
    sector %= NUM_SECTORS_PER_SLOT;
    // select save slot
    sector += NUM_SECTORS_PER_SLOT * (gSaveCounter % 2);

    data = locations[sectorId].data;
    size = locations[sectorId].size;

    // clear temp save section.
    for (i = 0; i < sizeof(struct SaveSector); i++)
        ((char *)gReadWriteSector)[i] = 0;

    gReadWriteSector->id = sectorId;
    gReadWriteSector->signature = SECTOR_SIGNATURE;
    gReadWriteSector->counter = gSaveCounter;

    // set temp section's data.
    for (i = 0; i < size; i++)
        gReadWriteSector->data[i] = data[i];

    // calculate checksum.
    gReadWriteSector->checksum = CalculateChecksum(data, size);

    EraseFlashSector(sector);

    status = SAVE_STATUS_OK;

    for (i = 0; i < sizeof(struct UnkSaveSection); i++)
    {
        if (ProgramFlashByte(sector, i, gReadWriteSector->data[i]))
        {
            status = SAVE_STATUS_ERROR;
            break;
        }
    }

    if (status == SAVE_STATUS_ERROR)
    {
        SetDamagedSectorBits(SECTOR_DAMAGED, sector);
        return SAVE_STATUS_ERROR;
    }
    else
    {
        status = SAVE_STATUS_OK;

        for (i = 0; i < 7; i++)
        {
            if (ProgramFlashByte(sector, 0xFF9 + i, ((u8 *)gReadWriteSector)[0xFF9 + i]))
            {
                status = SAVE_STATUS_ERROR;
                break;
            }
        }

        if (status == SAVE_STATUS_ERROR)
        {
            SetDamagedSectorBits(SECTOR_DAMAGED, sector);
            return SAVE_STATUS_ERROR;
        }
        else
        {
            SetDamagedSectorBits(SECTOR_OK, sector);
            return SAVE_STATUS_OK;
        }
    }
}

static u8 CopySectorSignatureByte(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u16 sector;

    // select sector number
    sector = sectorId + gLastWrittenSector - 1;
    sector %= NUM_SECTORS_PER_SLOT;
    // select save slot
    sector += NUM_SECTORS_PER_SLOT * (gSaveCounter % 2);

    if (ProgramFlashByte(sector, sizeof(struct UnkSaveSection), ((u8 *)gReadWriteSector)[sizeof(struct UnkSaveSection)]))
    {
        // sector is damaged, so enable the bit in gDamagedSaveSectors and restore the last written sector and save counter.
        SetDamagedSectorBits(SECTOR_DAMAGED, sector);
        gLastWrittenSector = gLastKnownGoodSector;
        gSaveCounter = gLastSaveCounter;
        return SAVE_STATUS_ERROR;
    }
    else
    {
        SetDamagedSectorBits(SECTOR_OK, sector);
        return SAVE_STATUS_OK;
    }
}

static u8 WriteSectorSignatureByte(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u16 sector;

    sector = sectorId + gLastWrittenSector - 1;
    sector %= NUM_SECTORS_PER_SLOT;
    sector += NUM_SECTORS_PER_SLOT * (gSaveCounter % 2);

    if (ProgramFlashByte(sector, sizeof(struct UnkSaveSection), 0x25))
    {
        // sector is damaged, so enable the bit in gDamagedSaveSectors and restore the last written sector and save counter.
        SetDamagedSectorBits(SECTOR_DAMAGED, sector);
        gLastWrittenSector = gLastKnownGoodSector;
        gSaveCounter = gLastSaveCounter;
        return SAVE_STATUS_ERROR;
    }
    else
    {
        SetDamagedSectorBits(SECTOR_OK, sector);
        return SAVE_STATUS_OK;
    }
}

static u8 TryLoadSaveSlot(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u8 status;
    gReadWriteSector = gSaveDataBuffer;
    if (sectorId != 0xFFFF)
    {
        status = SAVE_STATUS_ERROR;
    }
    else
    {
        status = GetSaveValidStatus(locations);
        CopySaveSlotData(0xFFFF, locations);
    }

    return status;
}

static u8 CopySaveSlotData(u16 sectorId, const struct SaveSectorLocation *locations)
{
    u16 i;
    u16 checksum;
    u16 slotOffset = NUM_SECTORS_PER_SLOT * (gSaveCounter % 2);
    u16 id;

    for (i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        ReadFlashSector(i + slotOffset, gReadWriteSector);
        id = gReadWriteSector->id;
        if (id == 0)
            gLastWrittenSector = i;
        checksum = CalculateChecksum(gReadWriteSector->data, locations[id].size);
        if (gReadWriteSector->signature == SECTOR_SIGNATURE
         && gReadWriteSector->checksum == checksum)
        {
            u16 j;
            for (j = 0; j < locations[id].size; j++)
                locations[id].data[j] = gReadWriteSector->data[j];
        }
    }

    return 1;
}

static u8 GetSaveValidStatus(const struct SaveSectorLocation *locations)
{
    u16 i;
    bool8 signatureValid;
    u16 checksum;
    u32 saveSlot1Counter = 0;
    u32 saveSlot2Counter = 0;
    u8 saveSlot1Status;
    u8 saveSlot2Status;
    u32 validSectorFlags;
    const u32 ALL_SECTORS = (1 << NUM_SECTORS_PER_SLOT) - 1;  // bitmask of all saveblock sectors

    // check save slot 1.
    validSectorFlags = 0;
    signatureValid = FALSE;
    for (i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        ReadFlashSector(i, gReadWriteSector);
        if (gReadWriteSector->signature == SECTOR_SIGNATURE)
        {
            signatureValid = TRUE;
            checksum = CalculateChecksum(gReadWriteSector->data, locations[gReadWriteSector->id].size);
            if (gReadWriteSector->checksum == checksum)
            {
                saveSlot1Counter = gReadWriteSector->counter;
                validSectorFlags |= 1 << gReadWriteSector->id;
            }
        }
    }

    if (signatureValid)
    {
        if (validSectorFlags == ALL_SECTORS)
            saveSlot1Status = SAVE_STATUS_OK;
        else
            saveSlot1Status = SAVE_STATUS_ERROR;
    }
    else
    {
        saveSlot1Status = SAVE_STATUS_EMPTY;
    }

    // check save slot 2.
    validSectorFlags = 0;
    signatureValid = FALSE;
    for (i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        ReadFlashSector(NUM_SECTORS_PER_SLOT + i, gReadWriteSector);
        if (gReadWriteSector->signature == SECTOR_SIGNATURE)
        {
            signatureValid = TRUE;
            checksum = CalculateChecksum(gReadWriteSector->data, locations[gReadWriteSector->id].size);
            if (gReadWriteSector->checksum == checksum)
            {
                saveSlot2Counter = gReadWriteSector->counter;
                validSectorFlags |= 1 << gReadWriteSector->id;
            }
        }
    }

    if (signatureValid)
    {
        if (validSectorFlags == ALL_SECTORS)
            saveSlot2Status = SAVE_STATUS_OK;
        else
            saveSlot2Status = SAVE_STATUS_ERROR;
    }
    else
    {
        saveSlot2Status = SAVE_STATUS_EMPTY;
    }

    if (saveSlot1Status == SAVE_STATUS_OK && saveSlot2Status == SAVE_STATUS_OK)
    {
        // Choose counter of the most recent save file
        if ((saveSlot1Counter == -1 && saveSlot2Counter == 0) || (saveSlot1Counter == 0 && saveSlot2Counter == -1))
        {
            if ((unsigned)(saveSlot1Counter + 1) < (unsigned)(saveSlot2Counter + 1))
                gSaveCounter = saveSlot2Counter;
            else
                gSaveCounter = saveSlot1Counter;
        }
        else
        {
            if (saveSlot1Counter < saveSlot2Counter)
                gSaveCounter = saveSlot2Counter;
            else
                gSaveCounter = saveSlot1Counter;
        }
        return SAVE_STATUS_OK;
    }

    if (saveSlot1Status == SAVE_STATUS_OK)
    {
        gSaveCounter = saveSlot1Counter;
        if (saveSlot2Status == SAVE_STATUS_ERROR)
            return SAVE_STATUS_ERROR;
        else
            return SAVE_STATUS_OK;
    }

    if (saveSlot2Status == SAVE_STATUS_OK)
    {
        gSaveCounter = saveSlot2Counter;
        if (saveSlot1Status == SAVE_STATUS_ERROR)
            return SAVE_STATUS_ERROR;
        else
            return SAVE_STATUS_OK;
    }

    if (saveSlot1Status == SAVE_STATUS_EMPTY && saveSlot2Status == SAVE_STATUS_EMPTY)
    {
        gSaveCounter = 0;
        gLastWrittenSector = 0;
        return SAVE_STATUS_EMPTY;
    }

    gSaveCounter = 0;
    gLastWrittenSector = 0;
    return 2;
}

static u8 TryLoadSaveSector(u8 sectorId, u8 *data, u16 size)
{
    u16 i;
    struct SaveSector *sector = gSaveDataBuffer;

    ReadFlashSector(sectorId, sector);
    if (sector->signature == SECTOR_SIGNATURE)
    {
        u16 checksum = CalculateChecksum(sector->data, size);
        if (sector->id == checksum)
        {
            for (i = 0; i < size; i++)
                data[i] = sector->data[i];
            return SAVE_STATUS_OK;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return SAVE_STATUS_EMPTY;
    }
}

static bool8 ReadFlashSector(u8 sectorId, struct SaveSector *sector)
{
    ReadFlash(sectorId, 0, sector->data, sizeof(struct SaveSector));
    return 1;
}

static u16 CalculateChecksum(void *data, u16 size)
{
    u16 i;
    u32 checksum = 0;

    for (i = 0; i < (size / 4); i++)
    {
        checksum += *((u32 *)data);
        data += sizeof(u32);
    }

    return ((checksum >> 16) + checksum);
}

#if DEBUG
void sub_813B79C(void)
{
    struct SaveSector *sbSector;
    struct SaveSector *hofSector;
    const struct SaveSectorLocation *sbChunks;
    const struct SaveSectorLocation *hofChunks;
    u16 i;

    sbSector = gSaveDataBuffer;
    sbChunks = sSaveBlockChunks;
    for (i = 0; i < NUM_SECTORS_PER_SLOT * 2; i++)
    {
        ReadFlashSector(i, sbSector);
        sbSector->checksum = CalculateChecksum(sbSector, sbChunks[sbSector->id].size);
        ProgramFlashSectorAndVerify(i, sbSector->data);
    }

    hofSector = gSaveDataBuffer;
    hofChunks = sHallOfFameChunks;
    for (i = 0; i < NUM_HOF_SECTORS; i++)
    {
        ReadFlashSector(SECTOR_ID_HOF_1 + i, hofSector);
        hofSector->id = CalculateChecksum(hofSector, hofChunks[i].size);  // why id?
        ProgramFlashSectorAndVerify(SECTOR_ID_HOF_1 + i, hofSector->data);
    }
}
#endif

u8 HandleSavingData(u8 saveType)
{
    u8 i;

    switch (saveType)
    {
    case SAVE_HALL_OF_FAME_ERASE_BEFORE: // wipes all hall of fame data, then saves hall of fame. unused
        for (i = SECTOR_ID_HOF_1; i < SECTORS_COUNT; i++)
            EraseFlashSector(i);
        // fall through
    case SAVE_HALL_OF_FAME: // hall of fame.
        if (GetGameStat(GAME_STAT_ENTERED_HOF) < 999)
            IncrementGameStat(GAME_STAT_ENTERED_HOF);
        for (i = 0; i < NUM_HOF_SECTORS; i++)
            HandleWriteSectorNBytes(SECTOR_ID_HOF_1 + i, sHallOfFameChunks[i].data, sHallOfFameChunks[i].size);
        CopyPartyAndObjectsToSave();
        WriteSaveSectorOrSlot(0xFFFF, sSaveBlockChunks);
        break;
    case SAVE_NORMAL: // normal save. also called by overwriting your own save.
    default:
        CopyPartyAndObjectsToSave();
        WriteSaveSectorOrSlot(0xFFFF, sSaveBlockChunks);
        break;
    case SAVE_LINK: // link save. updates only gSaveBlock1 and gSaveBlock2.
        CopyPartyAndObjectsToSave();
        for (i = 0; i < 5; i++)
            WriteSaveSectorOrSlot(i, sSaveBlockChunks);
        break;
    case SAVE_EREADER: // used in mossdeep "game corner" before/after battling old man e-reader trainer
        CopyPartyAndObjectsToSave();
        WriteSaveSectorOrSlot(0, sSaveBlockChunks);
        break;
    case SAVE_OVERWRITE_DIFFERENT_FILE: // there is a different file, so overwrite it completely.
        // Erase Hall of Fame.
        for (i = SECTOR_ID_HOF_1; i < SECTORS_COUNT; i++)
            EraseFlashSector(i);
        CopyPartyAndObjectsToSave();
        WriteSaveSectorOrSlot(0xFFFF, sSaveBlockChunks);
        break;
    }
    return 0;
}

#if (DEBUG && GERMAN)
extern u32 gUnknown_Debug_03004BD0;
#endif

u8 TrySavingData(u8 saveType) // TrySave
{
    if (gFlashMemoryPresent != TRUE)
        return SAVE_STATUS_ERROR;

    HandleSavingData(saveType);
    if (!gDamagedSaveSectors
#if (DEBUG && GERMAN)
        && gUnknown_Debug_03004BD0 == 0
#endif
        )
        return SAVE_STATUS_OK;

    DoSaveFailedScreen(saveType);
    return SAVE_STATUS_ERROR;
}

bool8 LinkFullSave_Init(void) // trade.s save
{
    if (gFlashMemoryPresent != TRUE)
        return 1;
    CopyPartyAndObjectsToSave();
    RestoreSaveBackupVarsAndIncrement(sSaveBlockChunks);
    return 0;
}

bool8 LinkFullSave_WriteSector(void) // trade.s save
{
    u8 status = HandleWriteIncrementalSector(ARRAY_COUNT(sSaveBlockChunks), sSaveBlockChunks);
    if (gDamagedSaveSectors)
        DoSaveFailedScreen(0);
    if (status == SAVE_STATUS_ERROR)
        return 1;
    else
        return 0;
}

bool8 LinkFullSave_ReplaceLastSector(void) // trade.s save
{
    HandleReplaceSectorAndVerify(ARRAY_COUNT(sSaveBlockChunks), sSaveBlockChunks);
    if (gDamagedSaveSectors)
        DoSaveFailedScreen(0);
    return 0;
}

bool8 LinkFullSave_SetLastSectorSignature(void) // trade.s save
{
    CopySectorSignatureByte(ARRAY_COUNT(sSaveBlockChunks), sSaveBlockChunks);
    if (gDamagedSaveSectors)
        DoSaveFailedScreen(0);
    return 0;
}

bool8 WriteSaveBlock2(void)
{
    if (gFlashMemoryPresent != TRUE)
        return 1;

    CopyPartyAndObjectsToSave();
    RestoreSaveBackupVars(sSaveBlockChunks);
    HandleReplaceSectorAndVerify(gIncrementalSectorId + 1, sSaveBlockChunks);
    return 0;
}

// something to do with multiplayer. Possibly record mizing?
bool8 WriteSaveBlock1Sector(void)
{
    u8 finished = FALSE;
    u16 sectorId = ++gIncrementalSectorId;
    if (sectorId <= 4)
    {
        HandleReplaceSectorAndVerify(gIncrementalSectorId + 1, sSaveBlockChunks);
        WriteSectorSignatureByte(sectorId, sSaveBlockChunks);
    }
    else
    {
        WriteSectorSignatureByte(sectorId, sSaveBlockChunks);
        finished = TRUE;
    }
    if (gDamagedSaveSectors)
        DoSaveFailedScreen(1);
    return finished;
}

u8 LoadGameSave(u8 saveType)
{
    u8 status;

    if (gFlashMemoryPresent != TRUE)
    {
        gSaveFileStatus = SAVE_STATUS_NO_FLASH;
        return SAVE_STATUS_ERROR;
    }

    switch (saveType)
    {
    case SAVE_NORMAL:
    default:
        status = TryLoadSaveSlot(0xFFFF, sSaveBlockChunks);
        CopyPartyAndObjectsFromSave();
        gSaveFileStatus = status;
        gGameContinueCallback = 0;
        break;
    case SAVE_HALL_OF_FAME:
        status = TryLoadSaveSector(SECTOR_ID_HOF_1, sHallOfFameChunks[0].data, sHallOfFameChunks[0].size);
        if (status == SAVE_STATUS_OK)
            status = TryLoadSaveSector(SECTOR_ID_HOF_1 + 1, sHallOfFameChunks[1].data, sHallOfFameChunks[1].size);
        break;
    }

    return status;
}

const u8 sUnusedFlashSectors[] = { 30, 31 };

bool8 unref_sub_8125F4C(struct UnkSaveSection *a1)
{
    u16 i;
    char *raw = (char *)a1;

    for (i = 0; i < sizeof(struct SaveSector); i++)
        raw[i] = 0;

    ReadFlash(sUnusedFlashSectors[0], 0, a1->data, 4096);

    if (a1->signature != SECTOR_SIGNATURE)
        return FALSE;

    return TRUE;
}

u8 unref_sub_8125FA0(void)
{
    u16 i;
    u8 status = TrySavingData(SAVE_NORMAL);

    for (i = 0; i < 2; i++)
        EraseFlashSector(sUnusedFlashSectors[i]);

    if (status == SAVE_STATUS_ERROR)
    {
        return 3;
    }
    else if (status == 3)
    {
        return 2;
    }
    else
    {
        LoadGameSave(SAVE_NORMAL);
        return 1;
    }
}

u8 unref_sub_8125FF0(u8 *data, u16 size)
{
    u16 i;
    struct UnkSaveSection *section = (struct UnkSaveSection *)gSaveDataBuffer;

    for (i = 0; i < sizeof(struct SaveSector); i++)
        ((char *)section)[i] = 0;

    section->signature = SECTOR_SIGNATURE;

    for (i = 0; i < size; i++)
        section->data[i] = data[i];

    gLastSaveSectorStatus = ProgramFlashSectorAndVerifyNBytes(sUnusedFlashSectors[0], section, sizeof(struct SaveSector));

    if (gLastSaveSectorStatus)
        return SAVE_STATUS_ERROR;
    else
        return SAVE_STATUS_OK;
}

u8 unref_sub_8126068(u8 sector, u8 *data, u32 size)
{
    if (ProgramFlashSectorAndVerify(sector, data))
        return SAVE_STATUS_ERROR;
    else
        return SAVE_STATUS_OK;
}

u8 unref_sub_8126080(u8 sector, u8 *data)
{
    ReadFlash(sector, 0, data, sizeof(struct SaveSector));
    return 1;
}

asm(".section .rodata\n"
    "\t.align 2\n"
    "\t.text");
