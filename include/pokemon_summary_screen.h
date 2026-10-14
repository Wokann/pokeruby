#ifndef GUARD_POKEMON_SUMMARY_SCREEN_H
#define GUARD_POKEMON_SUMMARY_SCREEN_H

#include "main.h"
#include "task.h"

extern u8 gLastViewedMonIndex;

extern const u8 *const gMoveDescriptionPointers[];
extern const u8 *const gNatureNamePointers[];

// The Pokemon Summary Screen can operate in different modes. Certain features,
// such as move re-ordering, are available in the different modes.
enum PokemonSummaryScreenMode
{
    SUMMARY_MODE_NORMAL,
    PSS_MODE_MOVES_ONLY,
    SUMMARY_MODE_SELECT_MOVE,
    PSS_MODE_MOVE_DELETER, // TODO: this mode might be used by pokemon_storage_system
    SUMMARY_MODE_LOCK_MOVES,
    PSS_MODE_PC_NORMAL,
    PSS_MODE_PC_MOVES_ONLY,
};

enum PokemonSummaryScreenPage
{
	PSS_PAGE_INFO,
	PSS_PAGE_SKILLS,
	PSS_PAGE_BATTLE_MOVES,
	PSS_PAGE_CONTEST_MOVES,
};

struct PokemonSummaryScreenData
{
    /*0x00*/ union {
        struct Pokemon *mons;
        struct BoxPokemon *boxMons;
    } monList;
    /*0x04*/ MainCallback callback;
    /*0x08*/ u8 mode; // see enum PokemonSummaryScreenMode
    /*0x09*/ u8 curMonIndex;
    /*0x0A*/ u8 maxMonIndex;
    /*0x0B*/ u8 currPageIndex; // enum PokemonSummaryScreenPage
    /*0x0C*/ u8 monSpriteId;
    /*0x0D*/ u8 ballSpriteId;
    /*0x0E*/ bool8 isBoxMon;
    /*0x0F*/ u8 inputHandlingTaskId;
    /*0x10*/ struct Pokemon currentMon;
    /*0x74*/ u8 loadGfxState;
    /*0x75*/ u8 minPageIndex;
    /*0x76*/ u8 maxPageIndex;
    /*0x77*/ u8 unk77;
    /*0x78*/ u8 unk78;
    /*0x79*/ u8 firstMoveIndex;
    /*0x7A*/ u8 secondMoveIndex;
    /*0x7B*/ bool8 lockMovesFlag;
    /*0x7C*/ u16 newMove;
    /*0x7E*/ u8 headerTextId; // used as index into sPageHeaderTexts
    /*0x7F*/ u8 headerActionTextId; // used as index into sPageHeaderTexts
    /*0x80*/ u8 bgDisplayOrder;
    /*0x84*/ TaskFunc unk84;
};

void ShowPokemonSummaryScreen(struct Pokemon *mons, u8 monIndex, u8 maxMonIndex, MainCallback callback, u8 mode);
void ShowSelectMovePokemonSummaryScreen(struct Pokemon *mons, u8 monIndex, u8 maxMonIndex, MainCallback callback, u16 newMove);
u8 GetMoveSlotToReplace(void);
u8 *PokemonSummaryScreen_CopyPokemonLevel(u8 *dest, u8 level);


#endif // GUARD_POKEMON_SUMMARY_SCREEN_H
