#include "global.h"
#include "ewram.h"
#include "random.h"
#include "task.h"
#include "contest.h"
#include "contest_link.h"
#include "text.h"
#include "string_util.h"
#include "link.h"

static void Task_LinkContest_StartInitFlags(u8 taskId);
static void Task_LinkContest_InitFlags(u8 taskId);
#if GERMAN
static void de_sub_80C9274(bool32 arg0);
static void de_sub_80C9294(bool32 arg0);
#endif

static void LinkContest_SendBlock(const void *src, u16 size)
{
    memcpy(gDecompressionBuffer, src, size);
    SendBlock(BitmaskAllOtherLinkPlayers(), gDecompressionBuffer, size);
}

static bool8 LinkContest_GetBlockReceived(u8 flag)
{
    u8 mask = 1 << flag;
    if (!(GetBlockReceivedStatus() & mask))
        return FALSE;
    ResetBlockReceivedFlag(mask);
    return TRUE;
}

static bool8 LinkContest_GetBlockReceivedFromAllPlayers(void)
{
    int i;

    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (!((GetBlockReceivedStatus() >> i) & 1))
            return FALSE;
    }
    ResetBlockReceivedFlags();
    return TRUE;
}

void Task_LinkContest_Init(u8 taskId)
{
#if ENGLISH
    u8 i;

    for (i = 0; i < 4; i++)
        gBlockRecvBuffer[i][0] = 0xff;
#endif
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].func = Task_LinkContest_StartInitFlags;
}

static void Task_LinkContest_StartInitFlags(u8 taskId)
{
    gTasks[taskId].func = Task_LinkContest_InitFlags;
}

static void Task_LinkContest_InitFlags(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers)
    {
        gContestPlayerMonIndex = GetMultiplayerId();
        if (GetLinkPlayerCount() == MAX_LINK_PLAYERS)
        {
            gLinkContestFlags = TRUE;
            SwitchTaskToFollowupFunc(taskId);
        }
    }
}

u8 GetMonNicknameLanguage(const u8 *nickname)
{
    u8 language = GAME_LANGUAGE;
    if (nickname[0] == EXT_CTRL_CODE_BEGIN && nickname[1] == 0x15)
        return language;
    if (StringLength(nickname) < PLAYER_NAME_LENGTH - 1)
    {
        while (*nickname != EOS)
        {
            if (((*nickname >= CHAR_A && *nickname <= CHAR_z) ||
                    (*nickname >= CHAR_0 + 0 && *nickname <= CHAR_0 + 9) ||
                    *nickname == CHAR_SPACE ||
                    *nickname == CHAR_PERIOD ||
                    *nickname == CHAR_COMMA ||
                    *nickname == CHAR_EXCL_MARK ||
                    *nickname == CHAR_QUESTION_MARK ||
                    *nickname == CHAR_MALE ||
                    *nickname == CHAR_FEMALE ||
                    *nickname == CHAR_SLASH ||
                    *nickname == CHAR_HYPHEN ||
                    *nickname == CHAR_ELLIPSIS ||
                    *nickname == CHAR_DBL_QUOTE_LEFT ||
                    *nickname == CHAR_DBL_QUOTE_RIGHT ||
                    *nickname == CHAR_SGL_QUOTE_LEFT ||
                    *nickname == CHAR_DBL_QUOTE_LEFT)) // Most likely a typo, CHAR_SGL_QUOTE_RIGHT should be here instead.
            {
                nickname++;
            }
            else
            {
                language = LANGUAGE_JAPANESE;
                break;
            }
        }
    }
    return language;
}

void Task_LinkContest_CommunicateMonsRS(u8 taskId)
{
    int i;
    u8 *name;

    switch (gTasks[taskId].data[0]) {
#if ENGLISH
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
#elif GERMAN
            case 8:
#endif
        case 0:
            if (GetMultiplayerId() == 0) {
                if (IsLinkTaskFinished()) {
#if ENGLISH
                    memcpy(gBlockSendBuffer, gContestMons + gContestPlayerMonIndex, sizeof(struct ContestPokemon));
                    SendBlockRequest(2);
                    gTasks[taskId].data[0]++;
#elif GERMAN
                    if (gTasks[taskId].data[0] == 0)
                    {
                        gTasks[taskId].data[0] = 3;
                    }
                    else
                    {
                        memcpy(gBlockSendBuffer, gContestMons + gContestPlayerMonIndex, sizeof(struct ContestPokemon));
                        de_sub_80C9274(FALSE);
                        SendBlockRequest(2);
                        gTasks[taskId].data[0] = 1;
                    }
#endif
                }
            }
            else
            {
                memcpy(gBlockSendBuffer, gContestMons + gContestPlayerMonIndex, sizeof(struct ContestPokemon));
#if GERMAN
                de_sub_80C9294(FALSE);
#endif
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                for (i = 0; i < MAX_LINK_PLAYERS; i++)
                {
                    memcpy(gContestMons + i, gBlockRecvBuffer[i], sizeof(struct ContestPokemon));
                    name = gContestMons[i].nickname;
                    if (gLinkPlayers[i].language == LANGUAGE_JAPANESE)
                    {
                        ConvertInternationalString(name, GetMonNicknameLanguage(name));
                    }
                    else if (name[10] == EXT_CTRL_CODE_BEGIN)
                    {
                        ConvertInternationalString(name, LANGUAGE_JAPANESE);
                    } else
                    {
                        name[5] = name[10];
                        name[10] = EOS;
                    }
                    name = gContestMons[i].trainerName;
                    if (gLinkPlayers[i].language == LANGUAGE_JAPANESE)
                    {
                        name[7] = EOS;
                        name[6] = name[4];
                        name[5] = name[3];
                        name[4] = name[2];
                        name[3] = name[1];
                        name[2] = name[0];
                        name[1] = 0x15;
                        name[0] = EXT_CTRL_CODE_BEGIN;
                    }
                    else
                    {
                        name[5] = name[7];
                        name[7] = EOS;
                    }
                }
                gTasks[taskId].data[0]++;
            }
            break;
#if GERMAN
        case 2:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        default:
            gTasks[taskId].data[0]++;
            break;
#endif
    }
}

void Task_LinkContest_CommunicateRngRS(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case 0:
            if (GetMultiplayerId() == 0)
            {
                if (IsLinkTaskFinished())
                {
                    LinkContest_SendBlock(&gRngValue, sizeof(u32));
                    gTasks[taskId].data[0]++;
                }
            }
            else
            {
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceived(0))
            {
                memcpy(&gRngValue, gBlockRecvBuffer[0], sizeof(u32));
                memcpy(&gContestRngValue, gBlockRecvBuffer[0], sizeof(u32));
                gTasks[taskId].data[0]++;
            }
            break;
    }
}

void Task_LinkContest_CommunicateCategoryRS(u8 taskId)
{
    int i;

    switch (gTasks[taskId].data[0])
    {
#if ENGLISH
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
#elif GERMAN
        case 8:
#endif
        case 0:
            gBlockSendBuffer[0] = gTasks[taskId].data[9];
            if (GetMultiplayerId() == 0)
            {
                if (IsLinkTaskFinished())
                {
#if ENGLISH
                    SendBlockRequest(2);
                    gTasks[taskId].data[0]++;
#elif GERMAN
                    if (gTasks[taskId].data[0] == 0)
                    {
                        gTasks[taskId].data[0] = 3;
                    }
                    else
                    {
                        de_sub_80C9274(TRUE);
                        SendBlockRequest(2);
                        gTasks[taskId].data[0] = 1;
                    }
#endif
                }
            }
            else
            {
#if GERMAN
                de_sub_80C9294(TRUE);
#endif
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                for (i = 0; i < MAX_LINK_PLAYERS; i++)
                {
                    gTasks[taskId].data[i + 1] = gBlockRecvBuffer[i][0];
                }
                gTasks[taskId].data[0]++;
            }
            break;
#if GERMAN
        case 2:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        default:
            gTasks[taskId].data[0]++;
            break;
#endif
    }
}

void Task_LinkContest_CommunicateMonIdxs(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case 0:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(&gContestPlayerMonIndex, sizeof(u8));
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                gTasks[taskId].data[0]++;
            }
            break;
    }
}

void Task_LinkContest_CommunicateMoveSelections(u8 taskId)
{
    int i;

    switch (gTasks[taskId].data[0])
    {
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case 0:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(&eContestantStatus[gContestPlayerMonIndex].currMove, sizeof(u16));
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                for (i = 0; i < MAX_LINK_PLAYERS; i++)
                {
                    *&eContestantStatus[i].currMove = gBlockRecvBuffer[i][0];
                }
                gTasks[taskId].data[0]++;
            }
            break;
    }
}

void Task_LinkContest_CommunicateFinalStandings(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(gContestMonTotalPoints, sizeof gContestMonTotalPoints);
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestMonTotalPoints, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestMonTotalPoints);
                gTasks[taskId].data[0]++;
            }
            break;
        case 2:
        case 5:
        case 8:
        case 11:
            if (gTasks[taskId].data[1]++ > 10)
            {
                gTasks[taskId].data[1] = 0;
                gTasks[taskId].data[0]++;
            }
            break;
        case 3:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(
                    gContestMonAppealPointTotals, sizeof gContestMonAppealPointTotals);
                gTasks[taskId].data[0]++;
            }
            break;
        case 4:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestMonAppealPointTotals, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestMonAppealPointTotals);
                gTasks[taskId].data[0]++;
            }
            break;
        case 6:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(gContestMonRound2Points, sizeof gContestMonRound2Points);
                gTasks[taskId].data[0]++;
            }
            break;
        case 7:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestMonRound2Points, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestMonRound2Points);
                gTasks[taskId].data[0]++;
            }
            break;
        case 9:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(gContestFinalStandings, sizeof gContestFinalStandings);
                gTasks[taskId].data[0]++;
            }
            break;
        case 10:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestFinalStandings, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestFinalStandings);
                gTasks[taskId].data[0]++;
            }
            break;
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
    }
}

void Task_LinkContest_CommunicateAppealsState(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(eContestantStatus, 4 * sizeof(struct ContestantStatus));
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(eContestantStatus, gBlockRecvBuffer[gContestLinkLeaderIndex], 4 * sizeof(struct ContestantStatus));
                gTasks[taskId].data[0]++;
            }
            break;
        case 2:
        case 5:
        case 8:
        case 11:
            if (gTasks[taskId].data[1]++ > 10)
            {
                gTasks[taskId].data[1] = 0;
                gTasks[taskId].data[0]++;
            }
            break;
        case 3:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(&eContestAppealResults, sizeof eContestAppealResults);
                gTasks[taskId].data[0]++;
            }
            break;
        case 4:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(&eContestAppealResults, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof eContestAppealResults);
                gTasks[taskId].data[0]++;
            }
            break;
        case 6:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(&eContestExcitement, sizeof eContestExcitement);
                gTasks[taskId].data[0]++;
            }
            break;
        case 7:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(&eContestExcitement, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof eContestExcitement);
                gTasks[taskId].data[0]++;
            }
            break;
        case 9:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(gContestantTurnOrder, sizeof gContestantTurnOrder);
                gTasks[taskId].data[0]++;
            }
            break;
        case 10:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestantTurnOrder, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestantTurnOrder);
                gTasks[taskId].data[0]++;
            }
            break;
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
    }
}

void Task_LinkContest_CommunicateLeaderIdsRS(u8 taskId)
{
    int i;

    switch (gTasks[taskId].data[0])
    {
#if ENGLISH
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
#elif GERMAN
        case 8:
#endif
        case 0:
            gBlockSendBuffer[0] = 0x64;
            if (GetMultiplayerId() == 0)
            {
                if (IsLinkTaskFinished())
                {
#if ENGLISH
                    SendBlockRequest(2);
                    gTasks[taskId].data[0]++;
#elif GERMAN
                    if (gTasks[taskId].data[0] == 0)
                    {
                        gTasks[taskId].data[0] = 3;
                    }
                    else
                    {
                        de_sub_80C9274(FALSE);
                        SendBlockRequest(2);
                        gTasks[taskId].data[0] = 1;
                    }
#endif
                }
            }
            else
            {
#if GERMAN
                de_sub_80C9294(FALSE);
#endif
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                for (i = 0; i < MAX_LINK_PLAYERS; i++)
                {
                    gTasks[taskId].data[5 + i] = gBlockRecvBuffer[i][0];
                }
                gTasks[taskId].data[0]++;
            }
            break;
#if GERMAN
        case 2:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        default:
            gTasks[taskId].data[0]++;
            break;
#endif
    }
}

void Task_LinkContest_CommunicateRound1Points(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case 0:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(gContestMonRound1Points, sizeof gContestMonRound1Points);
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestMonRound1Points, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestMonRound1Points);
                gTasks[taskId].data[0]++;
            }
            break;
    }
}

void Task_LinkContest_CommunicateTurnOrder(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        default:
            gTasks[taskId].data[0] = 0;
            SwitchTaskToFollowupFunc(taskId);
            break;
        case 0:
            if (IsLinkTaskFinished())
            {
                LinkContest_SendBlock(gContestantTurnOrder, sizeof gContestantTurnOrder);
                gTasks[taskId].data[0]++;
            }
            break;
        case 1:
            if (LinkContest_GetBlockReceivedFromAllPlayers())
            {
                memcpy(gContestantTurnOrder, gBlockRecvBuffer[gContestLinkLeaderIndex], sizeof gContestantTurnOrder);
                gTasks[taskId].data[0]++;
            }
            break;
    }
}

#if GERMAN

static void de_sub_80C9274(bool32 arg0)
{
    if (deUnkValue2 == 1)
    {
        if (arg0)
            deUnkValue2 = 3;
        else
            deUnkValue2 = 2;
    }
}

static void de_sub_80C9294(bool32 arg0)
{
    if (deUnkValue2 == 1)
    {
        if (arg0)
            deUnkValue2 = 3;
        else
            deUnkValue2 = 2;
    }
    else if (deUnkValue2 == 2)
    {
        SendBlock(0, sBlockRequests[deUnkValue1].address, sBlockRequests[deUnkValue1].size);
        if (arg0)
            deUnkValue2 = 0;
        else
            deUnkValue2 = 1;
    }
}

#endif
