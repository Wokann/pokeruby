.if DEBUG
#include "constants/decorations.h"
#include "constants/flags.h"
#include "constants/game_stat.h"
#include "constants/items.h"
#include "constants/event_objects.h"
#include "constants/maps.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/weather.h"
#include "constants/vars.h"
	.include "asm/macros.inc"
	.include "constants/constants.inc"
	.include "asm/macros/event.inc"

	.section .rodata

.ifdef ENGLISH
    .set MEVENT_LANGUAGE_MASK, 2
.else @ GERMAN
    .set MEVENT_LANGUAGE_MASK, 4
.endif @ GERMAN

.ifdef SAPPHIRE
    .set MEVENT_VERSION_MASK, 0x100
.else @ RUBY
    .set MEVENT_VERSION_MASK, 0x80
.endif @ RUBY

gDebugMysteryEventEonTicketScript::
	me_checkcompat gDebugMysteryEventEonTicketScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventEonTicketPayload, sDebugMysteryEventEonTicketPayloadEnd
sDebugMysteryEventEonTicketPayload:
	me_initramscript MAP_PETALBURG_CITY_GYM, 0x1, sDebugMysteryEventEonTicketRamScript, sDebugMysteryEventEonTicketRamScriptEnd
	me_setrecordmixinggift 0x1, 0x5, ITEM_EON_TICKET
	me_runscript sDebugMysteryEventEonTicketResultScript
	me_end

.ifdef ENGLISH
sText_DebugMysteryEventEonTicketCollect:
	.string "Go see your father at the GYM in\n"
	.string "PETALBURG.$"
.else @ GERMAN
sText_DebugMysteryEventEonTicketCollect:
	.string "Lauf und besuche deinen Vater in der\n"
	.string "ARENA von BLÜTENBURG CITY.$"
.endif @ GERMAN
sDebugMysteryEventEonTicketRamScript:
	setvaddress sDebugMysteryEventEonTicketRamScript
	checkitem ITEM_EON_TICKET, 0x1
	compare VAR_RESULT, 0x1
	vgoto_if 0x1, sDebugMysteryEventEonTicketRamScriptFinish
	checkpcitem ITEM_EON_TICKET, 0x1
	compare VAR_RESULT, 0x1
	vgoto_if 0x1, sDebugMysteryEventEonTicketRamScriptFinish
	checkflag FLAG_ENCOUNTERED_LATIAS_OR_LATIOS
	vgoto_if 0x1, sDebugMysteryEventEonTicketRamScriptFinish
	lock
	faceplayer
	vmessage sText_DebugMysteryEventEonTicketLetter
	waitmessage
	waitbuttonpress
	checkitemspace ITEM_EON_TICKET, 0x1
	compare VAR_RESULT, 0x0
	vgoto_if 0x1, sDebugMysteryEventEonTicketNoKeyItemSpace
	setorcopyvar VAR_0x8000, ITEM_EON_TICKET
	setorcopyvar VAR_0x8001, 0x1
	callstd 0x0
	setflag FLAG_SYS_HAS_EON_TICKET
	vmessage sText_DebugMysteryEventEonTicketReceived
	waitmessage
	waitbuttonpress
	release
sDebugMysteryEventEonTicketRamScriptFinish:
	endram
sDebugMysteryEventEonTicketNoKeyItemSpace:
	vmessage sText_DebugMysteryEventEonTicketKeyItemsFull
	waitmessage
	waitbuttonpress
	release
	end

.ifdef ENGLISH
sText_DebugMysteryEventEonTicketLetter:
	.string "DAD: {PLAYER}! Good to see you!\n"
	.string "There's a letter here for you, {PLAYER}.$"
sText_DebugMysteryEventEonTicketReceived:
	.string "DAD: It appears to be a ferry TICKET,\n"
	.string "but I've never seen one like it before.\p"
	.string "You should visit LILYCOVE and ask\n"
	.string "about it there.$"

sText_DebugMysteryEventEonTicketKeyItemsFull:
	.string "DAD: {PLAYER}, the KEY ITEMS POCKET in\n"
	.string "your BAG is full.\p"
	.string "Move some key items for safekeeping\n"
	.string "in your PC, then come see me.$"
.else @ GERMAN
sText_DebugMysteryEventEonTicketLetter:
	.string "VATER: {PLAYER}! Schön, dich zu sehen!\n"
	.string "Hier ist ein Brief für dich, {PLAYER}.$"
sText_DebugMysteryEventEonTicketReceived:
	.string "VATER: Ich bin mir nicht sicher, es\n"
	.string "könnte ein TICKET für eine Fähre sein.\p"
	.string "Du solltest nach SEEGRASULB CITY gehen\n"
	.string "und dich dort genauer erkundigen.$"

sText_DebugMysteryEventEonTicketKeyItemsFull:
	.string "VATER: {PLAYER}, die BASIS-TASCHE\n"
	.string "deines BEUTELS ist voll.\p"
	.string "Lagere einige deiner Basis-Items in\n"
	.string "deinem PC und komm dann wieder.$"
.endif @ GERMAN

sDebugMysteryEventEonTicketRamScriptEnd:
	.size sDebugMysteryEventEonTicketRamScript, sDebugMysteryEventEonTicketRamScriptEnd - sDebugMysteryEventEonTicketRamScript

sDebugMysteryEventEonTicketResultScript:
	setvaddress sDebugMysteryEventEonTicketResultScript
	checkitem ITEM_EON_TICKET, 0x1
	compare VAR_RESULT, 0x1
	vgoto_if 0x1, sDebugMysteryEventEonTicketAlreadyUsed
	checkpcitem ITEM_EON_TICKET, 0x1
	compare VAR_RESULT, 0x1
	vgoto_if 0x1, sDebugMysteryEventEonTicketAlreadyUsed
	checkflag FLAG_ENCOUNTERED_LATIAS_OR_LATIOS
	vgoto_if 0x1, sDebugMysteryEventEonTicketAlreadyUsed
	checkitemspace ITEM_EON_TICKET, 0x1
	compare VAR_RESULT, 0x0
	vgoto_if 0x1, sDebugMysteryEventEonTicketBagFull
	vbuffermessage sText_DebugMysteryEventEonTicketCollect
	setmysteryeventstatus MEVENT_STATUS_SUCCESS
	end

sDebugMysteryEventEonTicketAlreadyUsed:
	vbuffermessage sText_DebugMysteryEventEonTicketAlreadyUsed
	setmysteryeventstatus MEVENT_STATUS_FAILURE
	end

sDebugMysteryEventEonTicketBagFull:
	vbuffermessage sText_DebugMysteryEventEonTicketBagFull
	setmysteryeventstatus MEVENT_STATUS_FAILURE
	end

.ifdef ENGLISH
sText_DebugMysteryEventEonTicketAlreadyUsed:
	.string "This EVENT may be played only once.$"
sText_DebugMysteryEventEonTicketBagFull:
	.string "Your BAG's KEY ITEMS POCKET is full.$"
.else @ GERMAN
sText_DebugMysteryEventEonTicketAlreadyUsed:
	.string "Dieses GESCHEHEN kann nur einmal\n"
	.string "gespielt werden.$"
sText_DebugMysteryEventEonTicketBagFull:
	.string "Deine BASIS-TASCHE ist voll.$"
.endif @ GERMAN

sDebugMysteryEventEonTicketPayloadEnd:
	.size sDebugMysteryEventEonTicketPayload, sDebugMysteryEventEonTicketPayloadEnd - sDebugMysteryEventEonTicketPayload
gDebugMysteryEventEonTicketScriptEnd::

gDebugMysteryEventEnigmaBerryScript::
	me_checkcompat gDebugMysteryEventEnigmaBerryScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_checksum MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventEnigmaBerryPayload, sDebugMysteryEventEnigmaBerryPayloadEnd
sDebugMysteryEventEnigmaBerryPayload:
	me_setenigmaberry gDebugMysteryEventEnigmaBerryData
	me_end

gDebugMysteryEventEnigmaBerryData::
	.space 0x530
sDebugMysteryEventEnigmaBerryPayloadEnd:
	.size sDebugMysteryEventEnigmaBerryPayload, sDebugMysteryEventEnigmaBerryPayloadEnd - sDebugMysteryEventEnigmaBerryPayload
gDebugMysteryEventEnigmaBerryScriptEnd::

gDebugMysteryEventPokemonScript::
	me_checkcompat gDebugMysteryEventPokemonScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_runscript sDebugMysteryEventPokemonGiftResultScript
	me_setmsg MEVENT_STATUS_SUCCESS, sText_DebugMysteryEventPokemonGiftReceived
	me_setmsg MEVENT_STATUS_FAILURE, sText_DebugMysteryEventPokemonGiftPartyFull
	me_end

sDebugMysteryEventPokemonGiftResultScript:
	setvaddress sDebugMysteryEventPokemonGiftResultScript
	getpartysize
	compare VAR_RESULT, 0x6
	vgoto_if 0x1, sDebugMysteryEventPokemonGiftPartyFull
	givemon SPECIES_LUVDISC, 15, ITEM_BERRY_JUICE
	setmysteryeventstatus MEVENT_STATUS_SUCCESS
	end

sDebugMysteryEventPokemonGiftPartyFull:
	setmysteryeventstatus MEVENT_STATUS_FAILURE
	end

.ifdef ENGLISH
sText_DebugMysteryEventPokemonGiftReceived:
	.string "A POKéMON was added to the\n"
	.string "player's party.$"

sText_DebugMysteryEventPokemonGiftPartyFull:
	.string "The player's party is full.\n"
	.string "A POKéMON couldn't be added.$"
.else @ GERMAN
sText_DebugMysteryEventPokemonGiftReceived:
	.string "Ein POKéMON wurde in das Team des\n"
	.string "Spielers aufgenommen.$"

sText_DebugMysteryEventPokemonGiftPartyFull:
	.string "Das Team des Spielers ist vollständig.\n"
	.string "Es kann kein weiteres POKéMON\l"
	.string "aufgenommen werden.$"
.endif @ GERMAN

gDebugMysteryEventSpecialRibbonScript::
	me_checkcompat gDebugMysteryEventSpecialRibbonScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventSpecialRibbonPayload, sDebugMysteryEventSpecialRibbonPayloadEnd
sDebugMysteryEventSpecialRibbonPayload:
	me_giveribbon 0x0, 0x1
	me_end
sDebugMysteryEventSpecialRibbonPayloadEnd:
	.size sDebugMysteryEventSpecialRibbonPayload, sDebugMysteryEventSpecialRibbonPayloadEnd - sDebugMysteryEventSpecialRibbonPayload
gDebugMysteryEventSpecialRibbonScriptEnd::

gDebugMysteryEventNationalDexScript::
	me_checkcompat gDebugMysteryEventNationalDexScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventNationalDexPayload, sDebugMysteryEventNationalDexPayloadEnd
sDebugMysteryEventNationalDexPayload:
	me_givenationaldex
	me_end
sDebugMysteryEventNationalDexPayloadEnd:
	.size sDebugMysteryEventNationalDexPayload, sDebugMysteryEventNationalDexPayloadEnd - sDebugMysteryEventNationalDexPayload
gDebugMysteryEventNationalDexScriptEnd::

gDebugMysteryEventRareWordScript::
	me_checkcompat gDebugMysteryEventRareWordScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventRareWordPayload, sDebugMysteryEventRareWordPayloadEnd
sDebugMysteryEventRareWordPayload:
	me_addrareword 0x1
	me_end
sDebugMysteryEventRareWordPayloadEnd:
	.size sDebugMysteryEventRareWordPayload, sDebugMysteryEventRareWordPayloadEnd - sDebugMysteryEventRareWordPayload
gDebugMysteryEventRareWordScriptEnd::

gDebugMysteryEventPokemonGiftScript::
	me_checkcompat gDebugMysteryEventPokemonGiftScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventPokemonGiftPayload, sDebugMysteryEventPokemonGiftPayloadEnd
sDebugMysteryEventPokemonGiftPayload:
	me_givepokemon gDebugMysteryEventPokemonGiftData
	me_end

gDebugMysteryEventPokemonGiftData::
	.space 100

gDebugMysteryEventPokemonGiftMailData::
	.space 36
sDebugMysteryEventPokemonGiftPayloadEnd:
	.size sDebugMysteryEventPokemonGiftPayload, sDebugMysteryEventPokemonGiftPayloadEnd - sDebugMysteryEventPokemonGiftPayload
gDebugMysteryEventPokemonGiftScriptEnd::

gDebugMysteryEventEReaderTrainerScript::
	me_checkcompat gDebugMysteryEventEReaderTrainerScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_addtrainer gDebugMysteryEventEReaderTrainerData
	me_end

gDebugMysteryEventEReaderTrainerData::
	.space 256
gDebugMysteryEventEReaderTrainerScriptEnd::

gDebugMysteryEventResetRtcScript::
	me_checkcompat gDebugMysteryEventResetRtcScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_enableresetrtc
	me_end
gDebugMysteryEventResetRtcScriptEnd::

gDebugMysteryEventGanlonBerryScript::
	me_checkcompat gDebugMysteryEventGanlonBerryScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_checksum MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventGanlonBerryPayload, sDebugMysteryEventGanlonBerryPayloadEnd
sDebugMysteryEventGanlonBerryPayload:
	me_initramscript MAP_PETALBURG_CITY_GYM, 0x1, sDebugMysteryEventGanlonBerryRamScript, sDebugMysteryEventGanlonBerryRamScriptEnd
	me_setmsg MEVENT_STATUS_FF, sText_DebugMysteryEventGanlonBerryCollect
	me_setstatus MEVENT_STATUS_SUCCESS
	me_end

.ifdef ENGLISH
sText_DebugMysteryEventGanlonBerryCollect:
	.string "Go see your dad at the\n"
	.string "PETALBURG GYM.$"
.else @ GERMAN
sText_DebugMysteryEventGanlonBerryCollect:
	.string "Besuche deinen Vater in der ARENA\n"
	.string "von BLÜTENBURG CITY.$"
.endif @ GERMAN

sDebugMysteryEventGanlonBerryRamScript:
	setvaddress sDebugMysteryEventGanlonBerryRamScript
	checkitem ITEM_GANLON_BERRY, 0x1
	compare VAR_RESULT, 0x1
	vgoto_if 0x1, sDebugMysteryEventGanlonBerryAlreadyReceived
	checkpcitem ITEM_GANLON_BERRY, 0x1
	compare VAR_RESULT, 0x1
	vgoto_if 0x1, sDebugMysteryEventGanlonBerryAlreadyReceived
	lock
	faceplayer
	vmessage sText_DebugMysteryEventGanlonBerryReceived
	waitmessage
	waitbuttonpress
	setorcopyvar VAR_0x8000, ITEM_GANLON_BERRY
	setorcopyvar VAR_0x8001, 0x1
	callstd 0x0
	release
	endram
sDebugMysteryEventGanlonBerryAlreadyReceived:
	returnram

.ifdef ENGLISH
sText_DebugMysteryEventGanlonBerryReceived:
	.string "DAD: Hi, {PLAYER}!\p"
	.string "I just received a rare BERRY.\n"
	.string "I'd like you to have it.$"
.else @ GERMAN
sText_DebugMysteryEventGanlonBerryReceived:
	.string "VATER: Hi, {PLAYER}!\p"
	.string "Ich habe gerade diese seltene BEERE\n"
	.string "erhalten. Ich schenke sie dir!$"
.endif @GERMAN

sDebugMysteryEventGanlonBerryPayloadEnd:
	.size sDebugMysteryEventGanlonBerryPayload, sDebugMysteryEventGanlonBerryPayloadEnd - sDebugMysteryEventGanlonBerryPayload
sDebugMysteryEventGanlonBerryRamScriptEnd:
	.size sDebugMysteryEventGanlonBerryRamScript, sDebugMysteryEventGanlonBerryRamScriptEnd - sDebugMysteryEventGanlonBerryRamScript
gDebugMysteryEventGanlonBerryScriptEnd::

gDebugMysteryEventRegisteelDollScript::
	me_checkcompat gDebugMysteryEventRegisteelDollScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, sDebugMysteryEventRegisteelDollPayload, sDebugMysteryEventRegisteelDollPayloadEnd
sDebugMysteryEventRegisteelDollPayload:
	me_runscript sDebugMysteryEventRegisteelDollResultScript
	me_end

sDebugMysteryEventRegisteelDollResultScript:
	setvaddress sDebugMysteryEventRegisteelDollResultScript
	bufferdecorationname 0x0, DECOR_REGISTEEL_DOLL
	adddecoration DECOR_REGISTEEL_DOLL
	compare VAR_RESULT, 0x0
	vgoto_if 0x1, sDebugMysteryEventRegisteelDollNoRoom
	vbuffermessage sText_DebugMysteryEventRegisteelDollReceived
	setmysteryeventstatus MEVENT_STATUS_SUCCESS
	end

sDebugMysteryEventRegisteelDollNoRoom:
	vbuffermessage sText_DebugMysteryEventRegisteelDollNoRoom
	setmysteryeventstatus MEVENT_STATUS_FAILURE
	end

.ifdef ENGLISH
sText_DebugMysteryEventRegisteelDollReceived:
	.string "{STR_VAR_1} was received!$"
sText_DebugMysteryEventRegisteelDollNoRoom:
	.string "There was no room to accept\n"
	.string "{STR_VAR_1}...$"
.else @ GERMAN
sText_DebugMysteryEventRegisteelDollReceived:
	.string "{STR_VAR_1} wurde übergeben.$"
sText_DebugMysteryEventRegisteelDollNoRoom:
	.string "Es ist kein Platz für {STR_VAR_1}\n"
	.string "vorhanden...$"
.endif @ GERMAN

sDebugMysteryEventRegisteelDollPayloadEnd:
	.size sDebugMysteryEventRegisteelDollPayload, sDebugMysteryEventRegisteelDollPayloadEnd - sDebugMysteryEventRegisteelDollPayload
gDebugMysteryEventRegisteelDollScriptEnd::

gDebugMysteryEventRecordMixingGiftScript::
	me_checkcompat gDebugMysteryEventRecordMixingGiftScript, MEVENT_LANGUAGE_MASK, MEVENT_LANGUAGE_MASK, MEVENT_GAME_MASK_RUBY_SAPPHIRE, MEVENT_VERSION_MASK
	me_crc MEVENT_CHECK_VALUE_UNSET, gDebugMysteryEventRecordMixingGiftData, gDebugMysteryEventRecordMixingGiftDataEnd
gDebugMysteryEventRecordMixingGiftData::
	me_setrecordmixinggift 0x1, 0x3, ITEM_POTION
	me_end
gDebugMysteryEventRecordMixingGiftDataEnd:
	.size gDebugMysteryEventRecordMixingGiftData, gDebugMysteryEventRecordMixingGiftDataEnd - gDebugMysteryEventRecordMixingGiftData
gDebugMysteryEventRecordMixingGiftScriptEnd::

.endif @ DEBUG
