#ifndef GUARD_POKEDEX_CRY_SCREEN_H
#define GUARD_POKEDEX_CRY_SCREEN_H

struct CryScreenWindow
{
    u16 charBase;
    u8 screenBase;
    u8 paletteNo;
    u8 xPos;
    u8 yPos;
};

extern u8 gDexCryScreenState;

bool8 ShowPokedexCryScreen(struct CryScreenWindow *window, u8 arg1);
bool8 LoadCryWaveformWindow(struct CryScreenWindow *window, u8 bgId);
void UpdateCryWaveformWindow(u8 bgId);
void CryScreenPlayButton(u16 species);
void DestroyCryMeterNeedleSprite();

#endif // GUARD_POKEDEX_CRY_SCREEN_H
