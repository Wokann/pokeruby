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

const struct BardSoundTemplate (*const gBardSoundTemplatesTable[])[MAX_BARD_SOUNDS_PER_WORD] = {
    gBardSoundTemplates_Pokemon,
    gBardSoundTemplates_Trainer,
    gBardSoundTemplates_Status,
    gBardSoundTemplates_Battle,
    gBardSoundTemplates_Greetings,
    gBardSoundTemplates_People,
    gBardSoundTemplates_Voices,
    gBardSoundTemplates_Speech,
    gBardSoundTemplates_Endings,
    gBardSoundTemplates_Feelings,
    gBardSoundTemplates_Conditions,
    gBardSoundTemplates_Actions,
    gBardSoundTemplates_Lifestyle,
    gBardSoundTemplates_Hobbies,
    gBardSoundTemplates_Time,
    gBardSoundTemplates_Misc,
    gBardSoundTemplates_Adjectives,
    gBardSoundTemplates_Events,
    gBardSoundTemplates_Moves1,
    gBardSoundTemplates_Moves2,
    gBardSoundTemplates_TrendySaying,
    gBardSoundTemplates_Pokemon2
};

#endif //GUARD_BARD_SOUNDS_TABLE_H
