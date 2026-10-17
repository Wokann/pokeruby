#ifndef GUARD_INTERNATIONAL_STRING_UTIL_H
#define GUARD_INTERNATIONAL_STRING_UTIL_H

void ConvertInternationalPlayerName(u8 *str);
void TVShowConvertInternationalString(u8 *dest, const u8 *src, u8 language);
void PadNameString(u8 *dest, u8 padChar);

#endif // GUARD_INTERNATIONAL_STRING_UTIL_H
