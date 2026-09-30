#include "global.h"
#include "bard_music.h"
#include "easy_chat.h"

#include "data/bard_music/word_pitch.h"
#include "data/bard_music/length_table.h"
#include "data/bard_music/bard_sounds.h"

#define BASE_PITCH_TABLE_INDEX (NUM_BARD_PITCH_TABLES_PER_SIZE * MAX_BARD_SOUNDS_PER_WORD)

static s16 GetWordPitch(int tableIndex, int pitchIndex)
{
    return sPitchTables[tableIndex][pitchIndex];
}

#if ENGLISH
const struct BardSoundTemplate *GetWordSoundTemplates(u16 group, u16 word)
{
    const struct BardSoundTemplate (*soundTemplates)[MAX_BARD_SOUNDS_PER_WORD] = sBardSoundTemplatesTable[group];

    return soundTemplates[word];
}
#elif GERMAN
const struct BardSoundTemplate *GetWordSoundTemplates(u16 group, u16 word)
{
    const struct BardSoundTemplate (*soundTemplates)[MAX_BARD_SOUNDS_PER_WORD] = sBardSoundTemplatesTable[group];
    u32 index = GetEasyChatWordIndexInGroup(group, word);

    return soundTemplates[index];
}
#endif

s32 CalcWordSounds(struct BardSong *song, const struct BardSoundTemplate *soundTemplates, u16 pitchTableIndex)
{
    s32 i;
    s32 j;
    s32 basePitchTableIndex;

    for (i = 0; i < MAX_BARD_SOUNDS_PER_WORD; i++)
    {
        song->sounds[i].songId = soundTemplates[i].songId;
        if (soundTemplates[i].songId != PHONEME_ID_NONE)
        {
            s32 length = soundTemplates[i].lengthAdjustment + sPhonemeLengths[soundTemplates[i].songId];

            song->sounds[i].length = length;
            song->sounds[i].volume = soundTemplates[i].volume;
            song->length += length;
        }
    }

    for (j = 0, basePitchTableIndex = BASE_PITCH_TABLE_INDEX; j < i; j++)
        song->sounds[j].pitch = GetWordPitch(basePitchTableIndex + pitchTableIndex, j);

    song->lyricsIndex++;
    song->soundIndex = 0;
    song->timer = 0;
    song->state = 0;
    song->voiceInflection = 0;

    // Ruby's caller ignores this legacy return value.
}
