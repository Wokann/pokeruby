#include "global.h"
#include "battle.h"
#include "battle_tower.h"
#include "ewram.h"
#include "constants/trainers.h"

#if GERMAN

extern u8 gTrainerClassNames[][13];
extern struct Trainer gTrainers[];

u8 *GetGermanSchoolKidTrainerClassName(u8 gender)
{
    if (gender)
        gender++;
    gender = TRAINER_CLASS_SCHOOL_KID;
    return gTrainerClassNames[gender];
}

u8 *GetGermanPokemonTrainerClassName(u8 gender)
{
    if (gender)
        gender++;
    gender = TRAINER_CLASS_POKEMON_TRAINER_3;
    return gTrainerClassNames[gender];
}

u8 *GetGermanLeaderTrainerClassName(u8 gender)
{
    if (gender)
        gender++;
    gender = TRAINER_CLASS_LEADER;
    return gTrainerClassNames[gender];
}

#ifdef NONMATCHING

u8 GetBattleTowerTrainerFacilityClass(void);
u8 GetBattleTowerTrainerClassNameIndex(void);
u8 GetEReaderTrainerFacilityClass(void);

u8 *GetGermanTrainerClassName(s32 trainerSource, u32 trainerId)
{
    u8 trainerClassNameIndex, trainerClass, gender;
    struct Trainer *trainer;
    u8 encounterMusic;

    switch (trainerSource)
    {
    case SECRET_BASE_OPPONENT:
        trainerClassNameIndex = GetSecretBaseTrainerNameIndex();
        gender = sSecretBase->gender;
        if (trainerClassNameIndex == TRAINER_CLASS_SCHOOL_KID)
            return GetGermanSchoolKidTrainerClassName(gender);
        return gTrainerClassNames[trainerClassNameIndex];
    case BATTLE_TYPE_BATTLE_TOWER:
        trainerClass = GetBattleTowerTrainerFacilityClass();
        trainerClassNameIndex = GetBattleTowerTrainerClassNameIndex();
        if (trainerClass == FACILITY_CLASS_SCHOOL_KID_F)
            return GetGermanSchoolKidTrainerClassName(FEMALE);
        if (trainerClass == FACILITY_CLASS_MAY_1 || trainerClass == FACILITY_CLASS_MAY_2 || trainerClass == FACILITY_CLASS_MAY_3)
            return GetGermanPokemonTrainerClassName(FEMALE);
        return gTrainerClassNames[trainerClassNameIndex];
    case BATTLE_TYPE_EREADER_TRAINER:
        trainerClass = GetEReaderTrainerFacilityClass();
        trainerClassNameIndex = GetEReaderTrainerClassNameIndex();
        if (trainerClass == FACILITY_CLASS_SCHOOL_KID_F)
            return GetGermanSchoolKidTrainerClassName(FEMALE);
        if (trainerClass == FACILITY_CLASS_MAY_1 || trainerClass == FACILITY_CLASS_MAY_2 || trainerClass == FACILITY_CLASS_MAY_3)
            return GetGermanPokemonTrainerClassName(FEMALE);
        return gTrainerClassNames[trainerClassNameIndex];
    default:
        trainer = &gTrainers[trainerId];
        trainerClass = trainer->trainerClass;
        encounterMusic = GetTrainerEncounterMusicId(trainerId);
        if (trainerClass == FACILITY_CLASS_LEADER_F)
            return GetGermanSchoolKidTrainerClassName(encounterMusic);
        if (trainerClass == FACILITY_CLASS_BIRD_KEEPER && encounterMusic == FEMALE)
            return GetGermanPokemonTrainerClassName(FEMALE);
        if (trainerClass == FACILITY_CLASS_ELITE_FOUR_F)
        {
            if (gTrainers[trainerId].doubleBattle == TRUE)
                return GetGermanLeaderTrainerClassName(FEMALE);
            else
                return GetGermanLeaderTrainerClassName(MALE);
        }
        return gTrainerClassNames[trainerClass];
    }
}
#else

NAKED
void GetGermanTrainerClassName(void) {
    asm(".syntax unified\n\
    push {r4-r6,lr}\n\
    adds r2, r0, 0\n\
    adds r6, r1, 0\n\
    movs r0, 0x80\n\
    lsls r0, 3\n\
    cmp r2, r0\n\
    beq _0804104A\n\
    cmp r2, r0\n\
    bgt _08041040\n\
    movs r0, 0x80\n\
    lsls r0, 1\n\
    cmp r2, r0\n\
    beq _08041064\n\
    b _0804109C\n\
_08041040:\n\
    movs r0, 0x80\n\
    lsls r0, 4\n\
    cmp r2, r0\n\
    beq _08041086\n\
    b _0804109C\n\
_0804104A:\n\
    bl GetSecretBaseTrainerNameIndex\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    ldr r0, _08041060 @ =gSharedMem + 0x17000\n\
    ldrb r0, [r0, 0x1]\n\
    lsls r0, 27\n\
    lsrs r2, r0, 31\n\
    cmp r5, 0x1A\n\
    beq _080410B8\n\
    b _080410F8\n\
    .align 2, 0\n\
_08041060: .4byte gSharedMem + 0x17000\n\
_08041064:\n\
    bl GetBattleTowerTrainerFacilityClass\n\
    lsls r0, 24\n\
    lsrs r4, r0, 24\n\
    bl GetBattleTowerTrainerClassNameIndex\n\
_08041070:\n\
    lsls r0, 24\n\
    lsrs r5, r0, 24\n\
    cmp r4, 0x1E\n\
    beq _08041094\n\
    adds r0, r4, 0\n\
    subs r0, 0x3D\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    cmp r0, 0x2\n\
    bls _080410CC\n\
    b _080410F8\n\
_08041086:\n\
    bl GetEReaderTrainerFacilityClass\n\
    lsls r0, 24\n\
    lsrs r4, r0, 24\n\
    bl GetEReaderTrainerClassNameIndex\n\
    b _08041070\n\
_08041094:\n\
    movs r0, 0x1\n\
    bl GetGermanSchoolKidTrainerClassName\n\
    b _08041102\n\
_0804109C:\n\
    ldr r1, _080410C0 @ =gTrainers\n\
    lsls r4, r6, 2\n\
    adds r0, r4, r6\n\
    lsls r0, 3\n\
    adds r0, r1\n\
    ldrb r5, [r0, 0x1]\n\
    lsls r0, r6, 16\n\
    lsrs r0, 16\n\
    bl GetTrainerEncounterMusicId\n\
    lsls r0, 24\n\
    lsrs r2, r0, 24\n\
    cmp r5, 0x1A\n\
    bne _080410C4\n\
_080410B8:\n\
    adds r0, r2, 0\n\
    bl GetGermanSchoolKidTrainerClassName\n\
    b _08041102\n\
    .align 2, 0\n\
_080410C0: .4byte gTrainers\n\
_080410C4:\n\
    cmp r5, 0x2E\n\
    bne _080410D4\n\
    cmp r2, 0x1\n\
    bne _080410D4\n\
_080410CC:\n\
    movs r0, 0x1\n\
    bl GetGermanPokemonTrainerClassName\n\
    b _08041102\n\
_080410D4:\n\
    cmp r5, 0x19\n\
    bne _080410F8\n\
    ldr r0, _080410F4 @ =gTrainers\n\
    adds r1, r4, r6\n\
    lsls r1, 3\n\
    adds r1, r0\n\
    ldrb r0, [r1, 0x18]\n\
    movs r2, 0\n\
    cmp r0, 0x1\n\
    bne _080410EA\n\
    movs r2, 0x1\n\
_080410EA:\n\
    adds r0, r2, 0\n\
    bl GetGermanLeaderTrainerClassName\n\
    b _08041102\n\
    .align 2, 0\n\
_080410F4: .4byte gTrainers\n\
_080410F8:\n\
    movs r0, 0xD\n\
    adds r1, r5, 0\n\
    muls r1, r0\n\
    ldr r0, _08041108 @ =gTrainerClassNames\n\
    adds r0, r1, r0\n\
_08041102:\n\
    pop {r4-r6}\n\
    pop {r1}\n\
    bx r1\n\
    .align 2, 0\n\
_08041108: .4byte gTrainerClassNames\n\
    .syntax divided\n");
}
#endif

const u8 *GetGermanBattleString(u16 stringId, const u8 *battleString)
{
    return battleString;
}

#endif
