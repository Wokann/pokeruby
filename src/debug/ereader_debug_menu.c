#include "global.h"
#include "debug.h"
#include "constants/species.h"
#include "constants/items.h"
#include "string_util.h"
#include "new_game.h"
#include "load_save.h"
#include "start_menu.h"
#include "mystery_event_script.h"
#include "berry.h"
#include "mail.h"

#if DEBUG
const u8 sText_DebugMysteryEventEggNickname[] = DTR("タマゴ", "EGG");
#else
const u8 sText_DebugMysteryEventEggNickname[] = _("タマゴ");
#endif

#if DEBUG

extern u8 gDebugMysteryEventEonTicketScript[];
extern u8 gDebugMysteryEventEonTicketScriptEnd[];

size_t BuildDebugMysteryEventEonTicketScript(u8 *dest)
{
    size_t size = gDebugMysteryEventEonTicketScriptEnd - gDebugMysteryEventEonTicketScript;
    memcpy(dest, gDebugMysteryEventEonTicketScript, size);
    TrySetMysteryEventScriptCRC(dest, gDebugMysteryEventEonTicketScript);
    return size;
}

extern u8 gDebugMysteryEventSpecialRibbonScript[];
extern u8 gDebugMysteryEventSpecialRibbonScriptEnd[];

size_t BuildDebugMysteryEventSpecialRibbonScript(u8 *dest)
{
    size_t size = gDebugMysteryEventSpecialRibbonScriptEnd - gDebugMysteryEventSpecialRibbonScript;
    memcpy(dest, gDebugMysteryEventSpecialRibbonScript, size);
    TrySetMysteryEventScriptCRC(dest, gDebugMysteryEventSpecialRibbonScript);
    return size;
}

extern u8 gDebugMysteryEventRareWordScript[];
extern u8 gDebugMysteryEventRareWordScriptEnd[];

size_t BuildDebugMysteryEventRareWordScript(u8 *dest)
{
    size_t size = gDebugMysteryEventRareWordScriptEnd - gDebugMysteryEventRareWordScript;
    memcpy(dest, gDebugMysteryEventRareWordScript, size);
    TrySetMysteryEventScriptCRC(dest, gDebugMysteryEventRareWordScript);
    return size;
}

extern u8 gDebugMysteryEventNationalDexScript[];
extern u8 gDebugMysteryEventNationalDexScriptEnd[];

size_t BuildDebugMysteryEventNationalDexScript(u8 *dest)
{
    size_t size = gDebugMysteryEventNationalDexScriptEnd - gDebugMysteryEventNationalDexScript;
    memcpy(dest, gDebugMysteryEventNationalDexScript, size);
    TrySetMysteryEventScriptCRC(dest, gDebugMysteryEventNationalDexScript);
    return size;
}

extern u8 gDebugMysteryEventResetRtcScript[];
extern u8 gDebugMysteryEventResetRtcScriptEnd[];

size_t BuildDebugMysteryEventResetRtcScript(u8 *dest)
{
    size_t size = gDebugMysteryEventResetRtcScriptEnd - gDebugMysteryEventResetRtcScript;
    memcpy(dest, gDebugMysteryEventResetRtcScript, size);
    return size;
}

extern u8 gDebugMysteryEventGanlonBerryScript[];
extern u8 gDebugMysteryEventGanlonBerryScriptEnd[];

size_t BuildDebugMysteryEventGanlonBerryScript(u8 *dest)
{
    size_t size = gDebugMysteryEventGanlonBerryScriptEnd - gDebugMysteryEventGanlonBerryScript;
    memcpy(dest, gDebugMysteryEventGanlonBerryScript, size);
    TrySetMysteryEventScriptChecksum(dest, gDebugMysteryEventGanlonBerryScript);
    return size;
}

extern u8 gDebugMysteryEventRegisteelDollScript[];
extern u8 gDebugMysteryEventRegisteelDollScriptEnd[];

size_t BuildDebugMysteryEventRegisteelDollScript(u8 *dest)
{
    size_t size = gDebugMysteryEventRegisteelDollScriptEnd - gDebugMysteryEventRegisteelDollScript;
    memcpy(dest, gDebugMysteryEventRegisteelDollScript, size);
    TrySetMysteryEventScriptCRC(dest, gDebugMysteryEventRegisteelDollScript);
    return size;
}

extern u8 gDebugMysteryEventRecordMixingGiftScript[];
extern u8 gDebugMysteryEventRecordMixingGiftScriptEnd[];
extern u8 gDebugMysteryEventRecordMixingGiftData[];

size_t BuildDebugMysteryEventRecordMixingGiftScript(u8 *dest, u16 itemId)
{
    u8 *itemIdPtr;
    size_t size = gDebugMysteryEventRecordMixingGiftScriptEnd - gDebugMysteryEventRecordMixingGiftScript;
    memcpy(dest, gDebugMysteryEventRecordMixingGiftScript, size);
    itemIdPtr = gDebugMysteryEventRecordMixingGiftData - gDebugMysteryEventRecordMixingGiftScript + dest;
    itemIdPtr += 3;
    itemIdPtr[0] = itemId;
    itemIdPtr[1] = itemId >> 8;
    TrySetMysteryEventScriptCRC(dest, gDebugMysteryEventRecordMixingGiftScript);
    return size;
}

size_t BuildDebugMysteryEventPotionGiftScript(u8 *dest)
{
    return BuildDebugMysteryEventRecordMixingGiftScript(dest, ITEM_POTION);
}

size_t BuildDebugMysteryEventPokeBallGiftScript(u8 *dest)
{
    return BuildDebugMysteryEventRecordMixingGiftScript(dest, ITEM_POKE_BALL);
}

size_t BuildDebugMysteryEventCheriBerryGiftScript(u8 *dest)
{
    return BuildDebugMysteryEventRecordMixingGiftScript(dest, ITEM_CHERI_BERRY);
}

size_t BuildDebugMysteryEventFocusPunchGiftScript(u8 *dest)
{
    return BuildDebugMysteryEventRecordMixingGiftScript(dest, ITEM_TM01_FOCUS_PUNCH);
}

extern u8 gDebugMysteryEventEnigmaBerryScript[];
extern u8 gDebugMysteryEventEnigmaBerryScriptEnd[];
extern u8 gDebugMysteryEventEnigmaBerryData[];

const u8 sText_DebugMysteryEventExternal[] = DTR("ガイブ", "EXTERNAL");

size_t BuildDebugMysteryEventEnigmaBerryScript(u8 *dest)
{
    u8 *enigmaBerry = (u8 *)&gSaveBlock1.enigmaBerry;
    u8 *scriptEnigmaBerry = (u8 *)(dest - gDebugMysteryEventEnigmaBerryScript + gDebugMysteryEventEnigmaBerryData);
    size_t size = gDebugMysteryEventEnigmaBerryScriptEnd - gDebugMysteryEventEnigmaBerryScript;
    int i;

    DebugCreateEnigmaBerry(sText_DebugMysteryEventExternal, 0, 0);

    for (i = 0; i < 0x1000; i++)
        dest[i] = 0;

    for (i = 0; i < size; i++)
        dest[i] = gDebugMysteryEventEnigmaBerryScript[i];

    for (i = 0; i < sizeof(struct EnigmaBerry); i++)
        scriptEnigmaBerry[i] = enigmaBerry[i];

    ClearEnigmaBerries();
    TrySetMysteryEventScriptChecksum(dest, gDebugMysteryEventEnigmaBerryScript);
    return size;
}

const u8 sText_DebugMysteryEventOtName[] = _("ENGLISH");
const u8 sText_DebugMysteryEventNickname[] = _("TOURNAMENT");

void CreateDebugMysteryEventPokemon(struct Pokemon *mon, u16 species, u8 level, u16 itemId)
{
    u32 heldItem;
    ZeroMonData(mon);
    CreateMon(mon, species, level, 32, FALSE, 0, TRUE, 9999);
    SetMonData(mon, MON_DATA_OT_NAME, sText_DebugMysteryEventOtName);
    SetMonData(mon, MON_DATA_NICKNAME, sText_DebugMysteryEventNickname);
    heldItem = itemId;
    SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);
}

void CreateDebugMysteryEventMail(struct Pokemon *mon, struct Mail *mail)
{
    u16 itemId;
    ClearMail(mail);
    itemId = GetMonData(mon, MON_DATA_HELD_ITEM);
    if (ItemIsMail(itemId))
    {
        u8 *playerName = mail->playerName;
        u8 *trainerId = mail->trainerId;
        u8 otName[8];
        int i;

        for (i = 0; i < 9; i++)
            mail->words[i] = i + 1;

        GetMonData(mon, MON_DATA_OT_NAME, otName);
        StringCopyN(playerName, otName, OT_NAME_LENGTH + 1);

        write_word_to_mem(GetMonData(mon, MON_DATA_OT_ID), trainerId);

        mail->species = SpeciesToMailSpecies(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_PERSONALITY));
        mail->itemId = itemId;

    }
}

extern u8 gDebugMysteryEventPokemonGiftScript[];
extern u8 gDebugMysteryEventPokemonGiftScriptEnd[];
extern u8 gDebugMysteryEventPokemonGiftData[];
extern u8 gDebugMysteryEventPokemonGiftMailData[];

size_t BuildDebugMysteryEventEggScript(u8 *dest)
{
    struct Mail mail;
    u8 *template = gDebugMysteryEventPokemonGiftScript;
    u8 *templateEnd = gDebugMysteryEventPokemonGiftScriptEnd;
    struct Pokemon *pokemon = gEnemyParty;
    size_t size = templateEnd - template;
    u8 buffer[11];

    memcpy(dest, template, size);
    CreateDebugMysteryEventPokemon(pokemon, SPECIES_PIKACHU, 5, ITEM_NONE);
    buffer[0] = TRUE;
    SetMonData(pokemon, MON_DATA_IS_EGG, buffer);
    buffer[0] = 3;
    SetMonData(pokemon, MON_DATA_FRIENDSHIP, buffer);
    buffer[0] = 255;
    SetMonData(pokemon, MON_DATA_MET_LOCATION, buffer);

    StringCopyN(buffer, sText_DebugMysteryEventEggNickname, POKEMON_NAME_LENGTH + 1);
    SetMonData(pokemon, MON_DATA_NICKNAME, buffer);

    buffer[0] = LANGUAGE_JAPANESE;
    SetMonData(pokemon, MON_DATA_LANGUAGE, buffer);

    memcpy(gDebugMysteryEventPokemonGiftData - template + dest, pokemon, sizeof(struct Pokemon));

    memcpy(gDebugMysteryEventPokemonGiftMailData - template + dest, &mail, sizeof(struct Mail));

    return size;
}

size_t BuildDebugMysteryEventUnownScript(u8 *dest)
{
    struct Mail mail;
    u8 *template = gDebugMysteryEventPokemonGiftScript;
    u8 *templateEnd = gDebugMysteryEventPokemonGiftScriptEnd;
    struct Pokemon *pokemon = gEnemyParty;
    size_t size = templateEnd - template;

    memcpy(dest, template, size);
    CreateDebugMysteryEventPokemon(pokemon, SPECIES_UNOWN, 21, ITEM_DREAM_MAIL);
    CreateDebugMysteryEventMail(pokemon, &mail);

    memcpy(gDebugMysteryEventPokemonGiftData - template + dest, pokemon, sizeof(struct Pokemon));

    memcpy(gDebugMysteryEventPokemonGiftMailData - template + dest, &mail, sizeof(struct Mail));

    TrySetMysteryEventScriptCRC(dest, template);

    return size;
}

size_t BuildDebugMysteryEventInvalidUnownScript(u8 *dest)
{
    struct Mail mail;
    u8 *template = gDebugMysteryEventPokemonGiftScript;
    u8 *templateEnd = gDebugMysteryEventPokemonGiftScriptEnd;
    struct Pokemon *pokemon = gEnemyParty;
    size_t size = templateEnd - template;

    memcpy(dest, template, size);
    CreateDebugMysteryEventPokemon(pokemon, SPECIES_UNOWN, 21, ITEM_DREAM_MAIL);
    CreateDebugMysteryEventMail(pokemon, &mail);
    pokemon->box.checksum = 0;

    memcpy(gDebugMysteryEventPokemonGiftData - template + dest, pokemon, sizeof(struct Pokemon));

    memcpy(gDebugMysteryEventPokemonGiftMailData - template + dest, &mail, sizeof(struct Mail));

    TrySetMysteryEventScriptCRC(dest, template);

    return size;
}

void CreateDebugMysteryEventTrainerParty(void)
{
    CreateDebugMysteryEventPokemon(gPlayerParty + 0, SPECIES_CHARMELEON, 50, ITEM_QUICK_CLAW);
    CreateDebugMysteryEventPokemon(gPlayerParty + 1, SPECIES_WARTORTLE, 50, ITEM_FOCUS_BAND);
    CreateDebugMysteryEventPokemon(gPlayerParty + 2, SPECIES_IVYSAUR, 50, ITEM_LUM_BERRY);
}

extern u8 gDebugMysteryEventEReaderTrainerScript[];
extern u8 gDebugMysteryEventEReaderTrainerScriptEnd[];
extern u8 gDebugMysteryEventEReaderTrainerData[];

const u8 sText_DebugMysteryEventEReaderTrainerName[] = DTR("　そら　　カードイー", "eCARD TRAINER");

size_t BuildDebugMysteryEventEReaderTrainerScript(u8 *dest)
{
    u8 *template = gDebugMysteryEventEReaderTrainerScript;
    size_t size = gDebugMysteryEventEReaderTrainerScriptEnd - template;
    struct BattleTowerEReaderTrainer eReaderTrainer;

    memcpy(dest, template, size);
    SavePlayerParty();
    CreateDebugMysteryEventTrainerParty();
    CreateDebugEReaderTrainer(&eReaderTrainer, sText_DebugMysteryEventEReaderTrainerName + 5, 9999);
    LoadPlayerParty();

    memcpy(gDebugMysteryEventEReaderTrainerData - template + dest, &eReaderTrainer, sizeof(eReaderTrainer));

    return size;
}

const u8 sText_DebugMysteryEventKokko[] = KANA("ポケコッコ"); // TRN: kokko?
const u8 sText_DebugMysteryEventEonTicket[] = _("ticket");
const u8 sText_DebugMysteryEventPokemon[] = _("{PKMN}");
const u8 sText_DebugMysteryEventInvalidPokemon[] = _("wrong {PKMN}");
const u8 sText_DebugMysteryEventRibbon[] = _("ribbon");
const u8 sText_DebugMysteryEventNationalDex[] = _("DEX");
const u8 sText_DebugMysteryEventMessage[] = _("message");
const u8 sText_DebugMysteryEventEggGift[] = _("egg");
const u8 sText_DebugMysteryEventRamScript[] = _("RAM seed");
const u8 sText_DebugMysteryEventTrainer[] = _("trainer");
const u8 sText_DebugMysteryEventClock[] = _("clock");
const u8 sText_DebugMysteryEventItem[] = _("item");
const u8 sText_DebugMysteryEventGoods[] = _("goods");
const u8 sText_DebugMysteryEventPotion[] = _("potion");
const u8 sText_DebugMysteryEventPokeBall[] = _("ball");
const u8 sText_DebugMysteryEventCheriBerry[] = _("cheri berry");
const u8 sText_DebugMysteryEventFocusPunch[] = _("TM01");

const struct MysteryEventDebugDataSource gMysteryEventDebugDataSources[] = {
    {sText_DebugMysteryEventEonTicket, BuildDebugMysteryEventEonTicketScript},
    {sText_DebugMysteryEventPokemon, BuildDebugMysteryEventUnownScript},
    {sText_DebugMysteryEventInvalidPokemon, BuildDebugMysteryEventInvalidUnownScript},
    {sText_DebugMysteryEventRibbon, BuildDebugMysteryEventSpecialRibbonScript},
    {sText_DebugMysteryEventNationalDex, BuildDebugMysteryEventNationalDexScript},
    {sText_DebugMysteryEventMessage, BuildDebugMysteryEventRareWordScript},
    {sText_DebugMysteryEventEggGift, BuildDebugMysteryEventEggScript},
    {sText_DebugMysteryEventRamScript, BuildDebugMysteryEventEnigmaBerryScript},
    {sText_DebugMysteryEventTrainer, BuildDebugMysteryEventEReaderTrainerScript},
    {sText_DebugMysteryEventClock, BuildDebugMysteryEventResetRtcScript},
    {sText_DebugMysteryEventItem, BuildDebugMysteryEventGanlonBerryScript},
    {sText_DebugMysteryEventGoods, BuildDebugMysteryEventRegisteelDollScript},
    {sText_DebugMysteryEventPotion, BuildDebugMysteryEventPotionGiftScript},
    {sText_DebugMysteryEventPokeBall, BuildDebugMysteryEventPokeBallGiftScript},
    {sText_DebugMysteryEventCheriBerry, BuildDebugMysteryEventCheriBerryGiftScript},
    {sText_DebugMysteryEventFocusPunch, BuildDebugMysteryEventFocusPunchGiftScript}
};

extern u8 gDebugMysteryEventPokemonScript[];

const u8 gMysteryEventDebugDataSourceCount = 16;

u8 *const gDebugMysteryEventScriptTemplates[] = {
    gDebugMysteryEventEonTicketScript,
    gDebugMysteryEventEnigmaBerryScript,
    gDebugMysteryEventPokemonScript,
    gDebugMysteryEventSpecialRibbonScript,
    gDebugMysteryEventSpecialRibbonScript,
    gDebugMysteryEventNationalDexScript,
    gDebugMysteryEventNationalDexScript,
    gDebugMysteryEventRareWordScript,
    gDebugMysteryEventRareWordScript,
    gDebugMysteryEventPokemonGiftScript
};

#endif // DEBUG
