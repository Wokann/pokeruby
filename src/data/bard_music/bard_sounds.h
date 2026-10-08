#ifndef GUARD_BARD_SOUNDS_TABLE_H
#define GUARD_BARD_SOUNDS_TABLE_H

#define NULL_BARD_SOUND { 0xff }

#include "pokemon.h"
#include "trainer.h"
#include "status.h"
#include "battle.h"
#include "greetings.h"
#include "people.h"
#include "voices.h"
#include "speech.h"
#include "endings.h"
#include "feelings.h"
#include "conditions.h"
#include "actions.h"
#include "lifestyle.h"
#include "hobbies.h"
#include "time.h"
#include "misc.h"
#include "adjectives.h"
#include "events.h"
#include "moves.h"
#include "trendysaying.h"
#include "pokemon_2.h"

static const struct BardSound (*const sBardSoundTemplatesTable[])[6] = {
    gBardSounds_Pokemon,
    sBardSoundTemplates_Trainer,
    sBardSoundTemplates_Status,
    sBardSoundTemplates_Battle,
    sBardSoundTemplates_Greetings,
    sBardSoundTemplates_People,
    sBardSoundTemplates_Voices,
    sBardSoundTemplates_Speech,
    sBardSoundTemplates_Endings,
    sBardSoundTemplates_Feelings,
    sBardSoundTemplates_Conditions,
    sBardSoundTemplates_Actions,
    sBardSoundTemplates_Lifestyle,
    sBardSoundTemplates_Hobbies,
    sBardSoundTemplates_Time,
    sBardSoundTemplates_Misc,
    sBardSoundTemplates_Adjectives,
    sBardSoundTemplates_Events,
    gBardSounds_Moves1,
    gBardSounds_Moves2,
    sBardSoundTemplates_TrendySaying,
    gBardSounds_Pokemon2
};

#endif //GUARD_BARD_SOUNDS_TABLE_H
