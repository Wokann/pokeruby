#ifndef GUARD_FIELDCONTROLAVATAR_H
#define GUARD_FIELDCONTROLAVATAR_H

struct FieldInput
{
    u8 pressedAButton:1;
    u8 checkStandardWildEncounter:1;
    u8 pressedStartButton:1;
    u8 pressedSelectButton:1;
    u8 heldDirection:1;
    u8 heldDirection2:1;
    u8 tookStep:1;
    u8 pressedBButton:1;
    u8 debugCycleWarp:1;
    u8 debugRButtonHeld:1;
    u8 debugStartButton:1;
    u8 debugDiveWarp:1;
    u8 input_field_1_4:1;
    u8 input_field_1_5:1;
    u8 input_field_1_6:1;
    u8 input_field_1_7:1;
    u8 dpadDirection;
    u8 unused;
};

void FieldClearPlayerInput(struct FieldInput *input);
void FieldGetPlayerInput(struct FieldInput *input, u16 newKeys, u16 heldKeys);
int ProcessPlayerFieldInput(struct FieldInput *input);
const u8 *GetInteractedLinkPlayerScript(struct MapPosition *position, u8 metatileBehavior, u8 direction);
void ClearPoisonStepCounter(void);
void RestartWildEncounterImmunitySteps(void);
const u8 *GetCoordEventScriptAtMapPosition(struct MapPosition *position);
const u8 *GetObjectEventScriptPointerPlayerFacing(void);
u8 TrySetDiveWarp(void);
bool8 TryDoDiveWarp(struct MapPosition *position, u16 metatileBehavior);

#endif // GUARD_FIELDCONTROLAVATAR_H
