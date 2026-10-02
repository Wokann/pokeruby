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
    SUMMARY_MODE_MOVES_ONLY,
    SUMMARY_MODE_SELECT_MOVE,
    SUMMARY_MODE_MOVE_DELETER,
    SUMMARY_MODE_LOCK_MOVES,
    SUMMARY_MODE_BOX,
    SUMMARY_MODE_BOX_MOVES_ONLY,
};

struct PokemonSummaryScreenData
{
    /*0x00*/ union {
        struct Pokemon *partyMons;
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
    /*0x74*/ u8 loadState;
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
    /*0x84*/ TaskFunc savedTaskFunc;
};

void ShowPokemonSummaryScreen(struct Pokemon *, u8, u8, MainCallback, u8);
void ShowSelectMovePokemonSummaryScreen(struct Pokemon *, u8, u8, MainCallback, u16);
u8 GetMoveSlotToReplace(void);


#endif // GUARD_POKEMON_SUMMARY_SCREEN_H
