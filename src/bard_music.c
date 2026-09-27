#include "global.h"
#include "bard_music.h"
#include "easy_chat.h"

#include "data/bard_music/word_pitch.h"
#include "data/bard_music/length_table.h"
#include "data/bard_music/bard_sounds.h"

s16 GetWordPitch(int tableIndex, int pitchIndex)
{
    return gBardSoundPitchTables[tableIndex][pitchIndex];
}

#if ENGLISH
const struct BardSound *GetWordSounds(u16 group, u16 word)
{
    const struct BardSound (*sounds)[6] = gBardSoundsTable[group];

    return sounds[word];
}
#elif GERMAN
const struct BardSound *GetWordSounds(u16 group, u16 word)
{
    const struct BardSound (*sounds)[6] = gBardSoundsTable[group];
    u32 index = GetEasyChatWordIndexInGroup(group, word);

    return sounds[index];
}
#endif

s32 CalcWordPhonemes(struct BardSong *song, const struct BardSound *sounds, u16 pitchTableIndex)
{
    s32 i;
    s32 j;
    s32 basePitchTableIndex;

    for (i = 0; i < 6; i++)
    {
        song->phonemes[i].sound = sounds[i].var00;
        if (sounds[i].var00 != 0xFF)
        {
            s32 length = sounds[i].var01 + gBardSoundLengthTable[sounds[i].var00];

            song->phonemes[i].length = length;
            song->phonemes[i].volume = sounds[i].volume;
            song->var04 += length;
        }
    }

    for (j = 0, basePitchTableIndex = 30; j < i; j++)
        song->phonemes[j].pitch = GetWordPitch(basePitchTableIndex + pitchTableIndex, j);

    song->currWord++;
    song->currPhoneme = 0;
    song->phonemeTimer = 0;
    song->state = 0;
    song->voiceInflection = 0;

    //warning: no return statement in function returning non-void
}
