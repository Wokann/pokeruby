#include "global.h"
#include "international_string_util.h"
#include "string_util.h"
#include "text.h"

void PadNameString(u8 *dest, u8 padChar)
{
    u8 length;

    Text_StripExtCtrlCodes(dest);
    length = StringLength(dest);

    if (padChar == EXT_CTRL_CODE_BEGIN)
    {
        while (length < 6)
        {
            dest[length] = EXT_CTRL_CODE_BEGIN;
            dest[length + 1] = 7;
            length += 2;
        }
    }
    else
    {
        while (length < 6)
        {
            dest[length] = padChar;
            length++;
        }
    }

    dest[length] = EOS;
}

void ConvertInternationalPlayerName(u8 *str)
{
    if (StringLength(str) < 6)
        ConvertInternationalString(str, LANGUAGE_JAPANESE);
    else
        Text_StripExtCtrlCodes(str);
}
