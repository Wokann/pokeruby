#include "global.h"
#include <stddef.h>
#include "battle.h"
#include "data.h"
#include "battle_tower.h"
#include "ewram.h"
#include "constants/trainers.h"

#if GERMAN

const u8 *de_sub_8040FE0(u8 gender)
{
    if (gender)
        gender++;
    gender = TRAINER_CLASS_SCHOOL_KID;
    return gTrainerClassNames[gender];
}

const u8 *de_sub_8040FF4(u8 gender)
{
    if (gender)
        gender++;
    gender = TRAINER_CLASS_RIVAL;
    return gTrainerClassNames[gender];
}

const u8 *de_sub_804100C(u8 gender)
{
    if (gender)
        gender++;
    gender = TRAINER_CLASS_LEADER;
    return gTrainerClassNames[gender];
}

#ifdef NONMATCHING

u8 de_sub_81364AC(void);
u8 get_trainer_class_name_index(void);
u8 de_sub_81364F8(void);

const u8 *de_sub_8041024(s32 arg0, u32 arg1)
{
    u8 nameIndex, trainerClass, gender;
    const struct Trainer *trainer;
    u8 local2;

    switch (arg0)
    {
    case TRAINER_SECRET_BASE:
        nameIndex = GetSecretBaseTrainerClass();
        gender = eSecretBaseRecord.gender;
        if (nameIndex == TRAINER_CLASS_SCHOOL_KID)
            return de_sub_8040FE0(gender);
        return gTrainerClassNames[nameIndex];
    case BATTLE_TYPE_BATTLE_TOWER:
        trainerClass = de_sub_81364AC();
        nameIndex = get_trainer_class_name_index();
        if (trainerClass == FACILITY_CLASS_SCHOOL_KID_F)
            return de_sub_8040FE0(FEMALE);
        if (trainerClass == FACILITY_CLASS_MAY_1 || trainerClass == FACILITY_CLASS_MAY_2 || trainerClass == FACILITY_CLASS_MAY_3)
            return de_sub_8040FF4(FEMALE);
        return gTrainerClassNames[nameIndex];
    case BATTLE_TYPE_EREADER_TRAINER:
        trainerClass = de_sub_81364F8();
        nameIndex = GetEreaderTrainerClassId();
        if (trainerClass == FACILITY_CLASS_SCHOOL_KID_F)
            return de_sub_8040FE0(FEMALE);
        if (trainerClass == FACILITY_CLASS_MAY_1 || trainerClass == FACILITY_CLASS_MAY_2 || trainerClass == FACILITY_CLASS_MAY_3)
            return de_sub_8040FF4(FEMALE);
        return gTrainerClassNames[nameIndex];
    default:
        trainer = &gTrainers[arg1];
        trainerClass = trainer->trainerClass;
        local2 = GetTrainerEncounterMusicId(arg1);
        if (trainerClass == FACILITY_CLASS_LEADER_F)
            return de_sub_8040FE0(local2);
        if (trainerClass == FACILITY_CLASS_BIRD_KEEPER && local2 == FEMALE)
            return de_sub_8040FF4(FEMALE);
        if (trainerClass == FACILITY_CLASS_ELITE_FOUR_F)
        {
            if (gTrainers[arg1].doubleBattle == TRUE)
                return de_sub_804100C(FEMALE);
            else
                return de_sub_804100C(MALE);
        }
        return gTrainerClassNames[trainerClass];
    }
}
#else

// AGBCC permits ten asm operands. Expand macro constants as text,
// reserving operands for enum values and C layout expressions.
#define ASM_CONST_IMPL(value) #value
#define ASM_CONST(value) ASM_CONST_IMPL(value)

NAKED
void de_sub_8041024(void) {
    asm(
        ".syntax unified\n"
        "    push {r4-r6,lr}\n"
        "    adds r2, r0, 0\n"
        "    adds r6, r1, 0\n"
        "    movs r0, (" ASM_CONST(TRAINER_SECRET_BASE >> 3) ")\n"
        "    lsls r0, 3\n"
        "    cmp r2, r0\n"
        "    beq _0804104A\n"
        "    cmp r2, r0\n"
        "    bgt _08041040\n"
        "    movs r0, (" ASM_CONST(BATTLE_TYPE_BATTLE_TOWER >> 1) ")\n"
        "    lsls r0, 1\n"
        "    cmp r2, r0\n"
        "    beq _08041064\n"
        "    b _0804109C\n"
        "_08041040:\n"
        "    movs r0, (" ASM_CONST(BATTLE_TYPE_EREADER_TRAINER >> 4) ")\n"
        "    lsls r0, 4\n"
        "    cmp r2, r0\n"
        "    beq _08041086\n"
        "    b _0804109C\n"
        "_0804104A:\n"
        "    bl GetSecretBaseTrainerClass\n"
        "    lsls r0, 24\n"
        "    lsrs r5, r0, 24\n"
        "    ldr r0, _08041060 @ =eSecretBaseRecord\n"
        "    ldrb r0, [r0, 0x1]\n"
        "    lsls r0, 27\n"
        "    lsrs r2, r0, 31\n"
        "    cmp r5, %0 @ TRAINER_CLASS_SCHOOL_KID\n"
        "    beq _080410B8\n"
        "    b _080410F8\n"
        "    .align 2, 0\n"
        "_08041060: .4byte gSharedMem + %a9 @ eSecretBaseRecord\n"
        "_08041064:\n"
        "    bl de_sub_81364AC\n"
        "    lsls r0, 24\n"
        "    lsrs r4, r0, 24\n"
        "    bl get_trainer_class_name_index\n"
        "_08041070:\n"
        "    lsls r0, 24\n"
        "    lsrs r5, r0, 24\n"
        "    cmp r4, %1 @ FACILITY_CLASS_SCHOOL_KID_F\n"
        "    beq _08041094\n"
        "    adds r0, r4, 0\n"
        "    subs r0, %2 @ FACILITY_CLASS_MAY_1\n"
        "    lsls r0, 24\n"
        "    lsrs r0, 24\n"
        "    cmp r0, %3 @ FACILITY_CLASS_MAY_3 - FACILITY_CLASS_MAY_1\n"
        "    bls _080410CC\n"
        "    b _080410F8\n"
        "_08041086:\n"
        "    bl de_sub_81364F8\n"
        "    lsls r0, 24\n"
        "    lsrs r4, r0, 24\n"
        "    bl GetEreaderTrainerClassId\n"
        "    b _08041070\n"
        "_08041094:\n"
        "    movs r0, " ASM_CONST(FEMALE) "\n"
        "    bl de_sub_8040FE0\n"
        "    b _08041102\n"
        "_0804109C:\n"
        "    ldr r1, _080410C0 @ =gTrainers\n"
        "    lsls r4, r6, 2\n"
        "    adds r0, r4, r6\n"
        "    lsls r0, 3\n"
        "    adds r0, r1\n"
        "    ldrb r5, [r0, %8] @ Trainer.trainerClass\n"
        "    lsls r0, r6, 16\n"
        "    lsrs r0, 16\n"
        "    bl GetTrainerEncounterMusicId\n"
        "    lsls r0, 24\n"
        "    lsrs r2, r0, 24\n"
        "    cmp r5, %0 @ TRAINER_CLASS_SCHOOL_KID\n"
        "    bne _080410C4\n"
        "_080410B8:\n"
        "    adds r0, r2, 0\n"
        "    bl de_sub_8040FE0\n"
        "    b _08041102\n"
        "    .align 2, 0\n"
        "_080410C0: .4byte gTrainers\n"
        "_080410C4:\n"
        "    cmp r5, %4 @ TRAINER_CLASS_RIVAL\n"
        "    bne _080410D4\n"
        "    cmp r2, " ASM_CONST(TRAINER_ENCOUNTER_MUSIC_FEMALE) "\n"
        "    bne _080410D4\n"
        "_080410CC:\n"
        "    movs r0, " ASM_CONST(FEMALE) "\n"
        "    bl de_sub_8040FF4\n"
        "    b _08041102\n"
        "_080410D4:\n"
        "    cmp r5, %5 @ TRAINER_CLASS_LEADER\n"
        "    bne _080410F8\n"
        "    ldr r0, _080410F4 @ =gTrainers\n"
        "    adds r1, r4, r6\n"
        "    lsls r1, 3\n"
        "    adds r1, r0\n"
        "    ldrb r0, [r1, %6] @ Trainer.doubleBattle\n"
        "    movs r2, " ASM_CONST(MALE) "\n"
        "    cmp r0, " ASM_CONST(TRUE) "\n"
        "    bne _080410EA\n"
        "    movs r2, " ASM_CONST(FEMALE) "\n"
        "_080410EA:\n"
        "    adds r0, r2, 0\n"
        "    bl de_sub_804100C\n"
        "    b _08041102\n"
        "    .align 2, 0\n"
        "_080410F4: .4byte gTrainers\n"
        "_080410F8:\n"
        "    movs r0, %7 @ sizeof(gTrainerClassNames[0])\n"
        "    adds r1, r5, 0\n"
        "    muls r1, r0\n"
        "    ldr r0, _08041108 @ =gTrainerClassNames\n"
        "    adds r0, r1, r0\n"
        "_08041102:\n"
        "    pop {r4-r6}\n"
        "    pop {r1}\n"
        "    bx r1\n"
        "    .align 2, 0\n"
        "_08041108: .4byte gTrainerClassNames\n"
        "    .syntax divided\n"
        :
        : "i"(TRAINER_CLASS_SCHOOL_KID),
          "i"(FACILITY_CLASS_SCHOOL_KID_F),
          "i"(FACILITY_CLASS_MAY_1),
          "i"(FACILITY_CLASS_MAY_3 - FACILITY_CLASS_MAY_1),
          "i"(TRAINER_CLASS_RIVAL),
          "i"(TRAINER_CLASS_LEADER),
          "i"(offsetof(struct Trainer, doubleBattle)),
          "i"(sizeof(gTrainerClassNames[0])),
          "i"(offsetof(struct Trainer, trainerClass)),
          "i"((u8 *)eSecretBaseRecord - gSharedMem));
}
#endif

u32 de_sub_804110C(u32 arg0, u32 arg1)
{
    return arg1;
}

#endif
