#include "global.h"
#include "dewford_trend.h"
#include "easy_chat.h"
#include "constants/easy_chat.h"
#include "event_data.h"
#include "link.h"
#include "random.h"
#include "text.h"
#include "ewram.h"

extern u16 gSpecialVar_Result;
extern u16 gSpecialVar_0x8004;

enum {
    SORT_MODE_NORMAL,
    SORT_MODE_MAX_FIRST,
    SORT_MODE_FULL,
};

static void SortTrends(struct DewfordTrend *trends, u16 numTrends, u8 mode);
static bool8 CompareTrends(struct DewfordTrend *a, struct DewfordTrend *b, u8 mode);
static void SeedTrendRng(struct DewfordTrend *trend);
static bool8 IsPhraseInSavedTrends(u16 *phrase);
static bool8 IsEasyChatPairEqual(u16 *words1, u16 *words2);
static s16 GetSavedTrendIndex(struct DewfordTrend *trend, u16 numSaved);

void InitDewfordTrend(void)
{
    u16 i;

    for (i = 0; i < SAVED_TRENDS_COUNT; i++)
    {
        gSaveBlock1.dewfordTrends[i].words[0] = GetRandomEasyChatWordFromGroup(EC_GROUP_CONDITIONS);

        if (Random() & 1)
            gSaveBlock1.dewfordTrends[i].words[1] = GetRandomEasyChatWordFromGroup(EC_GROUP_LIFESTYLE);
        else
            gSaveBlock1.dewfordTrends[i].words[1] = GetRandomEasyChatWordFromGroup(EC_GROUP_HOBBIES);

        gSaveBlock1.dewfordTrends[i].gainingTrendiness = Random() & 1;
        SeedTrendRng(&gSaveBlock1.dewfordTrends[i]);
    }
    SortTrends(gSaveBlock1.dewfordTrends, SAVED_TRENDS_COUNT, SORT_MODE_NORMAL);
}

void UpdateDewfordTrendPerDay(u16 days)
{
    u16 i;

    if (days != 0)
    {
        u32 clockRand = days * 5;

        for (i = 0; i < SAVED_TRENDS_COUNT; i++)
        {
            //_080FA24A
            u32 trendiness;
            u32 rand = clockRand;
            struct DewfordTrend *trend = &gSaveBlock1.dewfordTrends[i];

            if (trend->gainingTrendiness == 0)
            {
                if (trend->trendiness >= (u16)rand)
                {
                    trend->trendiness -= rand;
                    if (trend->trendiness == 0)
                        trend->gainingTrendiness = 1;
                    continue;
                }
                //_080FA290
                rand -= trend->trendiness;
                trend->trendiness = 0;
                trend->gainingTrendiness = 1;
            }
            //_080FA2A0
            trendiness = trend->trendiness + rand;
            if ((u16)trendiness > trend->maxTrendiness)
            {
                u32 newTrendiness = trendiness % trend->maxTrendiness;
                trendiness = trendiness / trend->maxTrendiness;

                trend->gainingTrendiness = trendiness ^ 1;
                if (trend->gainingTrendiness)
                    trend->trendiness = newTrendiness;
                else
                //_080FA2FA
                    trend->trendiness = trend->maxTrendiness - newTrendiness;
            }
            else
            {
                //_080FA310
                trend->trendiness = trendiness;

                if (trend->trendiness == trend->maxTrendiness)
                    trend->gainingTrendiness = 0;
            }
        }
        SortTrends(gSaveBlock1.dewfordTrends, SAVED_TRENDS_COUNT, SORT_MODE_NORMAL);
    }
    //_080FA34E
}

bool8 TrySetTrendyPhrase(u16 *phrase)
{
    struct DewfordTrend trend = {0};
    u16 i;

    if (!IsPhraseInSavedTrends(phrase))
    {
        if (!FlagGet(FLAG_SYS_POPWORD_INPUT))
        {
            FlagSet(FLAG_SYS_POPWORD_INPUT);
            if (!FlagGet(FLAG_SYS_MIX_RECORD))
            {
                gSaveBlock1.dewfordTrends[0].words[0] = phrase[0];
                gSaveBlock1.dewfordTrends[0].words[1] = phrase[1];
                return TRUE;
            }
        }

        //_080FA3C8
        trend.words[0] = phrase[0];
        trend.words[1] = phrase[1];
        trend.gainingTrendiness = 1;
        SeedTrendRng(&trend);

        for (i = 0; i < SAVED_TRENDS_COUNT; i++)
        {
            if (CompareTrends(&trend, &gSaveBlock1.dewfordTrends[i], SORT_MODE_NORMAL))
            {
                u16 j;

                for (j = SAVED_TRENDS_COUNT - 1; j > i; j--)
                {
                    gSaveBlock1.dewfordTrends[j] = gSaveBlock1.dewfordTrends[j - 1];
                }
                gSaveBlock1.dewfordTrends[i] = trend;
                // i == SAVED_TRENDS_COUNT - 1 in Emerald
                return (i == 0);
            }
            //_080FA450
        }
        gSaveBlock1.dewfordTrends[SAVED_TRENDS_COUNT - 1] = trend;
    }
    return FALSE;
}

static void SortTrends(struct DewfordTrend *trends, u16 numTrends, u8 mode)
{
    u16 i;

    for (i = 0; i < numTrends; i++)
    {
        u16 j;

        for (j = i + 1; j < numTrends; j++)
        {
            if (CompareTrends(&trends[j], &trends[i], mode))
            {
                struct DewfordTrend temp;

                temp = trends[j];
                trends[j] = trends[i];
                trends[i] = temp;
            }
        }
    }
}

void ReceiveDewfordTrendData(struct DewfordTrend *linkedTrends, size_t size, u8 unused)
{
    u16 i;
    u16 j;
    u16 numTrends;
    struct DewfordTrend *src;
    struct DewfordTrend *dst;
    u16 players = GetLinkPlayerCount();

    for (i = 0; i < players; i++)
        memcpy(&eLinkedDewfordTrendsBuffer[i * SAVED_TRENDS_COUNT], (u8 *)linkedTrends + i * size, sizeof(struct DewfordTrend) * SAVED_TRENDS_COUNT);
    src = eLinkedDewfordTrendsBuffer;
    dst = eSavedDewfordTrendsBuffer;
    numTrends = 0;
    for (i = 0; i < players; i++)
    {
        for (j = 0; j < SAVED_TRENDS_COUNT; j++)
        {
            s16 idx = GetSavedTrendIndex(src, numTrends);
            if (idx < 0)
            {
                *(dst++) = *src;
                numTrends++;
            }
            else
            {
                if (eSavedDewfordTrendsBuffer[idx].trendiness < src->trendiness)
                {
                    eSavedDewfordTrendsBuffer[idx] = *src;
                }
            }
            src++;
        }
    }
    SortTrends(eSavedDewfordTrendsBuffer, numTrends, SORT_MODE_FULL);
    src = eSavedDewfordTrendsBuffer;
    dst = gSaveBlock1.dewfordTrends;
    for (i = 0; i < SAVED_TRENDS_COUNT; i++)
        *(dst++) = *(src++);
}

void BufferTrendyPhraseString(void)
{
    struct DewfordTrend *trend = &gSaveBlock1.dewfordTrends[gSpecialVar_0x8004];

    ConvertEasyChatWordsToString(gStringVar1, trend->words, 2, 1);
}

void IsTrendyPhraseBoring(void)
{
    u16 result = 0;

    do
    {
        if (gSaveBlock1.dewfordTrends[0].trendiness - gSaveBlock1.dewfordTrends[1].trendiness > 1)
            break;
        if (gSaveBlock1.dewfordTrends[0].gainingTrendiness)
            break;
        if (!gSaveBlock1.dewfordTrends[1].gainingTrendiness)
            break;
        result = 1;
    } while (0);

    gSpecialVar_Result = result;
}

void GetDewfordHallPaintingNameIndex(void)
{
    gSpecialVar_Result = (gSaveBlock1.dewfordTrends[0].words[0] + gSaveBlock1.dewfordTrends[0].words[1]) & 7;
}

static bool8 CompareTrends(struct DewfordTrend *a, struct DewfordTrend *b, u8 mode)
{
    switch (mode)
    {
    case SORT_MODE_NORMAL:
        if (a->trendiness > b->trendiness)
            return TRUE;
        if (a->trendiness < b->trendiness)
            return FALSE;
        if (a->maxTrendiness > b->maxTrendiness)
            return TRUE;
        if (a->maxTrendiness < b->maxTrendiness)
            return FALSE;
        break;
    case SORT_MODE_MAX_FIRST:
        if (a->maxTrendiness > b->maxTrendiness)
            return TRUE;
        if (a->maxTrendiness < b->maxTrendiness)
            return FALSE;
        if (a->trendiness > b->trendiness)
            return TRUE;
        if (a->trendiness < b->trendiness)
            return FALSE;
        break;
    case SORT_MODE_FULL:
        if (a->trendiness > b->trendiness)
            return TRUE;
        if (a->trendiness < b->trendiness)
            return FALSE;
        if (a->maxTrendiness > b->maxTrendiness)
            return TRUE;
        if (a->maxTrendiness < b->maxTrendiness)
            return FALSE;
        if (a->rand > b->rand)
            return TRUE;
        if (a->rand < b->rand)
            return FALSE;
        if (a->words[0] > b->words[0])
            return TRUE;
        if (a->words[0] < b->words[0])
            return FALSE;
        if (a->words[1] > b->words[1])
            return TRUE;
        if (a->words[1] < b->words[1])
            return FALSE;
        return TRUE;
    }
    return Random() & 1;
}

static void SeedTrendRng(struct DewfordTrend *trend)
{
    u16 rand;

    rand = Random() % 98;
    if (rand > 50)
    {
        rand = Random() % 98;
        if (rand > 80)
            rand = Random() % 98;
    }
    trend->maxTrendiness = rand + 30;
    trend->trendiness = (Random() % (rand + 1)) + 30;
    trend->rand = Random();
}

static bool8 IsPhraseInSavedTrends(u16 *phrase)
{
    u16 i;

    for (i = 0; i < SAVED_TRENDS_COUNT; i++)
    {
        if (IsEasyChatPairEqual(phrase, gSaveBlock1.dewfordTrends[i].words) != 0)
            return TRUE;
    }
    return FALSE;
}

static bool8 IsEasyChatPairEqual(u16 *words1, u16 *words2)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        if (*(words1++) != *(words2++))
            return FALSE;
    }
    return TRUE;
}

static s16 GetSavedTrendIndex(struct DewfordTrend *trend, u16 numSaved)
{
    s16 i;
    struct DewfordTrend *savedTrends = eSavedDewfordTrendsBuffer;

    for (i = 0; i < numSaved; i++)
    {
        if (IsEasyChatPairEqual(trend->words, savedTrends->words))
            return i;
        savedTrends++;
    }
    return -1;
}
