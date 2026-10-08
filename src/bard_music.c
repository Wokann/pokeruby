#include "global.h"
#include "bard_music.h"
#include "easy_chat.h"

#include "data/bard_music/word_pitch.h"
#include "data/bard_music/length_table.h"
#include "data/bard_music/bard_sounds.h"

static s16 GetWordPitch(int tableIndex, int pitchIndex)
{
    return sPitchTables[tableIndex][pitchIndex];
}

#if ENGLISH
const struct BardSound *GetWordSounds(u16 group, u16 word)
{
    const struct BardSound (*sounds)[6] = sBardSoundTemplatesTable[group];

    return sounds[word];
}
#elif GERMAN
const struct BardSound *GetWordSounds(u16 group, u16 word)
{
    const struct BardSound (*sounds)[6] = sBardSoundTemplatesTable[group];
    u32 index = de_sub_80EB748(group, word);

    return sounds[index];
}
#endif

s32 GetWordPhonemes(struct BardSong *song, const struct BardSound *soundTemplates, u16 pitchTableIndex)
{
    s32 i;
    s32 j;
    s32 thirty;

    for (i = 0; i < 6; i++)
    {
        song->sounds[i].sound = soundTemplates[i].songId;
        if (soundTemplates[i].songId != 0xFF)
        {
            s32 length = soundTemplates[i].lengthAdjustment + sPhonemeLengths[soundTemplates[i].songId];

            song->sounds[i].length = length;
            song->sounds[i].volume = soundTemplates[i].volume;
            song->length += length;
        }
    }

    for (j = 0, thirty = 30; j < i; j++)
        song->sounds[j].pitch = GetWordPitch(thirty + pitchTableIndex, j);

    song->lyricsIndex++;
    song->soundIndex = 0;
    song->timer = 0;
    song->state = 0;
    song->voiceInflection = 0;

    //warning: no return statement in function returning non-void
}
