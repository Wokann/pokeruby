#include "global.h"
#include "international_string_util.h"
#include "string_util.h"
#include "text.h"

void PadNameString(u8 *dest, u8 padChar)
{
    u8 length;

    StripExtCtrlCodes(dest);
    length = StringLength(dest);

    if (padChar == EXT_CTRL_CODE_BEGIN)
    {
        while (length < PLAYER_NAME_LENGTH - 1)
        {
            dest[length] = EXT_CTRL_CODE_BEGIN;
            dest[length + 1] = EXT_CTRL_CODE_RESET_FONT;
            length += 2;
        }
    }
    else
    {
        while (length < PLAYER_NAME_LENGTH - 1)
        {
            dest[length] = padChar;
            length++;
        }
    }

    dest[length] = EOS;
}

void ConvertInternationalPlayerName(u8 *str)
{
    if (StringLength(str) < PLAYER_NAME_LENGTH - 1)
        ConvertInternationalString(str, LANGUAGE_JAPANESE);
    else
        StripExtCtrlCodes(str);
}
