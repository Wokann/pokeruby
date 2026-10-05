#include "global.h"
#include "mail.h"
#include "constants/items.h"
#include "name_string_util.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "constants/species.h"
#include "text.h"

void ClearAllMail(void)
{
    u8 i;

    for (i = 0; i < 16; i++)
        ClearMail(&gSaveBlock1.mail[i]);
}

void ClearMail(struct Mail *mail)
{
    int i;

    for (i = 0; i < 9; i++)
        mail->words[i] = -1;

    for (i = 0; i < 8; i++)
        mail->playerName[i] = -1;

    for (i = 0; i < 4; i++)
        mail->trainerId[i] = 0;

    mail->species = 1;
    mail->itemId = 0;
}

bool8 MonHasMail(struct Pokemon *mon)
{
    u16 heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    if (ItemIsMail(heldItem) && GetMonData(mon, MON_DATA_MAIL) != 0xFF)
        return TRUE;
    else
        return FALSE;
}

u8 GiveMailToMonByItemId(struct Pokemon *mon, u16 itemId)
{
    u16 _itemId;
    u8 heldItem[2];
    u8 id;
    u8 i;
    u16 species;
    u32 personality;

    _itemId = itemId;
    heldItem[0] = _itemId;
    heldItem[1] = _itemId >> 8;
    id = 0;

    while (id < 6)
    {
        if (gSaveBlock1.mail[id].itemId == 0)
        {
            for (i = 0; i < 9; i++)
                gSaveBlock1.mail[id].words[i] = -1;

            for (i = 0; i < 7; i++)
                gSaveBlock1.mail[id].playerName[i] = gSaveBlock2.playerName[i];
            gSaveBlock1.mail[id].playerName[i] = EOS;
            PadNameString(gSaveBlock1.mail[id].playerName, 0);

            for (i = 0; i < 4; i++)
                gSaveBlock1.mail[id].trainerId[i] = gSaveBlock2.playerTrainerId[i];

            species = GetBoxMonData(&mon->box, MON_DATA_SPECIES);
            personality = GetBoxMonData(&mon->box, MON_DATA_PERSONALITY);
            gSaveBlock1.mail[id].species = SpeciesToMailSpecies(species, personality);
            gSaveBlock1.mail[id].itemId = _itemId;
            SetMonData(mon, MON_DATA_MAIL, &id);
            SetMonData(mon, MON_DATA_HELD_ITEM, heldItem);
            return id;
        }
        id++;
    }

    return -1;
}

u16 SpeciesToMailSpecies(u16 species, u32 personality)
{
    if (species == SPECIES_UNOWN)
    {
        int mailSpecies = GetUnownLetterByPersonality(personality) + 30000;
        return mailSpecies;
    }

    return species;
}

u16 MailSpeciesToSpecies(u16 mailSpecies, u16 *buffer)
{
    u16 result;

    if (mailSpecies >= 30000 && mailSpecies < (30000 + NUM_UNOWN_FORMS))
    {
        result = SPECIES_UNOWN;
        *buffer = mailSpecies - 30000;
    }
    else
    {
        result = mailSpecies;
    }

    return result;
}

u8 GiveMailToMon(struct Pokemon *mon, struct Mail *mail)
{
    u8 heldItem[2];
    u16 itemId = mail->itemId;
    u8 mailId = GiveMailToMonByItemId(mon, itemId);

    if (mailId == 0xFF)
        return 0xFF;

    gSaveBlock1.mail[mailId] = *mail;

    SetMonData(mon, MON_DATA_MAIL, &mailId);

    heldItem[0] = itemId;
    heldItem[1] = itemId >> 8;

    SetMonData(mon, MON_DATA_HELD_ITEM, heldItem);

    return mailId;
}

static int UNUSED DummyMailFunc(void)
{
    return 0;
}

void TakeMailFromMon(struct Pokemon *mon)
{
    u8 heldItem[2];
    u8 mailId;

    if (MonHasMail(mon))
    {
        mailId = GetMonData(mon, MON_DATA_MAIL);
        gSaveBlock1.mail[mailId].itemId = 0;
        mailId = 0xFF;
        heldItem[0] = 0;
        heldItem[1] = 0;
        SetMonData(mon, MON_DATA_MAIL, &mailId);
        SetMonData(mon, MON_DATA_HELD_ITEM, heldItem);
    }
}

void ClearMailItemId(u8 mailId)
{
    gSaveBlock1.mail[mailId].itemId = 0;
}

u8 TakeMailFromMonAndSave(struct Pokemon *mon)
{
    u8 i;
    u8 newHeldItem[2];
    u8 newMailId;

    newHeldItem[0] = 0;
    newHeldItem[1] = 0;
    newMailId = 0xFF;

    for (i = 6; i < 16; i++)
    {
        if (gSaveBlock1.mail[i].itemId == 0)
        {
            memcpy(&gSaveBlock1.mail[i], &gSaveBlock1.mail[GetMonData(mon, MON_DATA_MAIL)], sizeof(struct Mail));
            gSaveBlock1.mail[GetMonData(mon, MON_DATA_MAIL)].itemId = 0;
            SetMonData(mon, MON_DATA_MAIL, &newMailId);
            SetMonData(mon, MON_DATA_HELD_ITEM, newHeldItem);
            return i;
        }
    }

    return 0xFF;
}

bool8 ItemIsMail(u16 itemId)
{
    switch (itemId)
    {
    case ITEM_ORANGE_MAIL:
    case ITEM_HARBOR_MAIL:
    case ITEM_GLITTER_MAIL:
    case ITEM_MECH_MAIL:
    case ITEM_WOOD_MAIL:
    case ITEM_WAVE_MAIL:
    case ITEM_BEAD_MAIL:
    case ITEM_SHADOW_MAIL:
    case ITEM_TROPIC_MAIL:
    case ITEM_DREAM_MAIL:
    case ITEM_FAB_MAIL:
    case ITEM_RETRO_MAIL:
        return TRUE;
    default:
        return FALSE;
    }
}
