#include "constants/songs.h"
#include "global.h"
#include "menu.h"
#include "mystery_event_script.h"
#include "sound.h"
#include "text.h"

void ShowMysteryEventScriptResult(void);

bool8 RunMysteryEventScriptAndHandleResult(u8 *script)
{
    u8 status = RunMysteryEventScript(script);

    switch (status)
    {
    case MEVENT_STATUS_LOAD_OK:
        return FALSE;
    case MEVENT_STATUS_LOAD_ERROR:
        PlaySE(SE_BOO);
        return TRUE;
    case MEVENT_STATUS_SUCCESS:
        ShowMysteryEventScriptResult();
        return FALSE;
    case MEVENT_STATUS_FAILURE:
        PlaySE(SE_BOO);
        ShowMysteryEventScriptResult();
        return TRUE;
    default:
        return FALSE;
    }
}

void ShowMysteryEventScriptResult(void)
{
    Menu_DrawStdWindowFrame(2, 14, 27, 19);
    Menu_PrintText(gStringVar4, 3, 15);
}
