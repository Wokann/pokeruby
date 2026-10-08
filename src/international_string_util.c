#include "global.h"
#include "international_string_util.h"
#include "string_util.h"
#include "text.h"

void PadNameString(u8 *dest, u8 padChar)
{
    u8 length;

    StripExtCtrlCodes(dest);
    length = StringLength(dest);

    if (padChar == 0xFC)
    {
        while (length < 6)
        {
            dest[length] = 0xFC;
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
        ConvertInternationalString(str, 1);
    else
        StripExtCtrlCodes(str);
}
