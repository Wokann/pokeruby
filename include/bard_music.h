#ifndef GUARD_BARD_MUSIC_H
#define GUARD_BARD_MUSIC_H

// Exported type declarations

#define MAX_BARD_SOUNDS_PER_WORD 6
#define BARD_SOUND_ID_NONE 0xFF

struct BardSoundTemplate
{
    /*0x00*/ u8 songId;
    /*0x01*/ s8 lengthAdjustment;
    /*0x02*/ u16 unused;
    /*0x04*/ s16 volume;
    /*0x06*/ u16 unused2;
};

struct BardSound
{
    /*0x00*/ u16 songId;
    /*0x02*/ u16 length;
    /*0x04*/ s16 pitch;
    /*0x06*/ u16 volume;
};

struct BardSong
{
    /*0x00*/ u8 lyricsIndex;
    /*0x01*/ u8 soundIndex;
    /*0x02*/ u8 timer;
    /*0x03*/ u8 state;
    /*0x04*/ s16 length;
    /*0x06*/ u16 volume;
    /*0x08*/ s16 pitch;
    /*0x0A*/ s16 voiceInflection;
    /*0x0C*/ u16 lyrics[NUM_BARD_SONG_WORDS];
    /*0x18*/ struct BardSound sounds[MAX_BARD_SOUNDS_PER_WORD];
};

// Exported RAM declarations

extern struct BardSong gBardSong;

// Exported ROM declarations

const struct BardSoundTemplate *GetWordSoundTemplates(u16 group, u16 word);
s32 CalcWordSounds(struct BardSong *song, const struct BardSoundTemplate *soundTemplates, u16 pitchTableIndex);

#endif  // GUARD_BARD_MUSIC_H
