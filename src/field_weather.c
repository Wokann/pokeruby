#include "global.h"
#include "constants/songs.h"
#include "constants/weather.h"
#include "util.h"
#include "event_object_movement.h"
#include "field_weather.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "random.h"
#include "script.h"
#include "start_menu.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "ewram.h"
#include "constants/field_weather.h"

#define DROUGHT_COLOR_INDEX(color) ((((color) >> 1) & 0xF) | (((color) >> 2) & 0xF0) | (((color) >> 3) & 0xF00))

enum
{
    COLOR_MAP_NONE,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_CONTRAST,
};

struct RGBColor
{
    u16 r:5;
    u16 g:5;
    u16 b:5;
};

struct WeatherPaletteData
{
    u16 gammaShiftColors[8][0x1000]; // 0x1000 is the number of bytes that make up all palettes.
};

struct WeatherCallbacks
{
    void (*initVars)(void);
    void (*main)(void);
    void (*initAll)(void);
    bool8 (*finish)(void);
};

EWRAM_DATA struct Weather gWeather = {0};
EWRAM_DATA static u8 sFieldEffectPaletteColorMapTypes[32] = {0};
EWRAM_DATA u16 gUnknown_0202FF58 = 0;

static const u8 *sPaletteColorMapTypes;

const u8 DroughtPaletteData_0[] = INCBIN_U8("graphics/weather/drought0.bin.lz");
const u8 DroughtPaletteData_1[] = INCBIN_U8("graphics/weather/drought1.bin.lz");
const u8 DroughtPaletteData_2[] = INCBIN_U8("graphics/weather/drought2.bin.lz");
const u8 DroughtPaletteData_3[] = INCBIN_U8("graphics/weather/drought3.bin.lz");
const u8 DroughtPaletteData_4[] = INCBIN_U8("graphics/weather/drought4.bin.lz");
const u8 DroughtPaletteData_5[] = INCBIN_U8("graphics/weather/drought5.bin.lz");

static const u8 *const sCompressedDroughtPalettes[] =
{
    DroughtPaletteData_0,
    DroughtPaletteData_1,
    DroughtPaletteData_2,
    DroughtPaletteData_3,
    DroughtPaletteData_4,
    DroughtPaletteData_5,
    (u8*)eDroughtPaletteData.gammaShiftColors,
};

// This is a pointer to gWeather. All code in this file accesses gWeather directly,
// while code in other field weather files accesses gWeather through this pointer.
// This is likely the result of compiler optimization, since using the pointer in
// this file produces the same result as accessing gWeather directly.
struct Weather *const gWeatherPtr = &gWeather;

static bool8 LightenSpritePaletteInFog(u8);
static void BuildColorMaps(void);
static void UpdateWeatherColorMap(void);
static void ApplyColorMap(u8 startPalIndex, u8 numPalettes, s8 colorMapIndex);
static void ApplyColorMapWithBlend(u8 startPalIndex, u8 numPalettes, s8 colorMapIndex, u8 blendCoeff, u16 blendColor);
static void ApplyDroughtColorMapWithBlend(s8 colorMapIndex, u8 blendCoeff, u16 blendColor);
static void ApplyFogBlend(u8 blendCoeff, u16 blendColor);
static bool8 FadeInScreen_RainShowShade(void);
static bool8 FadeInScreen_Drought(void);
static bool8 FadeInScreen_FogHorizontal(void);
static void FadeInScreenWithWeather(void);
static void DoNothing(void);
static void Task_WeatherInit(u8 taskId);
static void Task_WeatherMain(u8 taskId);
static void None_Init(void);
static void None_Main(void);
static u8 None_Finish(void);

static const struct WeatherCallbacks sWeatherFuncs[] =
{
    {None_Init,          None_Main,      None_Init,         None_Finish},
    {Clouds_InitVars,    Clouds_Main,    Clouds_InitAll,    Clouds_Finish},
    {Sunny_InitVars,  Sunny_Main,  Sunny_InitAll,  Sunny_Finish},
    {Rain_InitVars, Rain_Main, Rain_InitAll, Rain_Finish},
    {Snow_InitVars,      Snow_Main,      Snow_InitAll,      Snow_Finish},
    {Thunderstorm_InitVars,   Thunderstorm_Main,      Thunderstorm_InitAll,   Thunderstorm_Finish},
    {FogHorizontal_InitVars,      FogHorizontal_Main,      FogHorizontal_InitAll,      FogHorizontal_Finish},
    {Ash_InitVars,       Ash_Main,       Ash_InitAll,       Ash_Finish},
    {Sandstorm_InitVars, Sandstorm_Main, Sandstorm_InitAll, Sandstorm_Finish},
    {FogDiagonal_InitVars,      FogDiagonal_Main,      FogDiagonal_InitAll,      FogDiagonal_Finish},
    {FogHorizontal_InitVars,      FogHorizontal_Main,      FogHorizontal_InitAll,      FogHorizontal_Finish},
    {Shade_InitVars,     Shade_Main,     Shade_InitAll,     Shade_Finish},
    {Drought_InitVars,   Drought_Main,   Drought_InitAll,   Drought_Finish},
    {Downpour_InitVars, Thunderstorm_Main,      Downpour_InitAll, Thunderstorm_Finish},
    {Bubbles_InitVars,   Bubbles_Main,   Bubbles_InitAll,   Bubbles_Finish},
};

void (*const gWeatherPalStateFuncs[])(void) =
{
    UpdateWeatherColorMap, // WEATHER_PAL_STATE_CHANGING_WEATHER
    FadeInScreenWithWeather, // WEATHER_PAL_STATE_SCREEN_FADING_IN
    DoNothing,               // WEATHER_PAL_STATE_SCREEN_FADING_OUT
    DoNothing,               // WEATHER_PAL_STATE_IDLE
};

// This table specifies which of the color maps should be
// applied to each of the background and sprite palettes.
static const u8 sBasePaletteColorMapTypes[32] =
{
    // background palettes
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_NONE,
    COLOR_MAP_NONE,
    // sprite palettes
    COLOR_MAP_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_CONTRAST,
    COLOR_MAP_CONTRAST,
    COLOR_MAP_CONTRAST,
    COLOR_MAP_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
    COLOR_MAP_DARK_CONTRAST,
};

#if DEBUG

static const u8 sDebugText_Weather_None[]        = DTR("なし　　　", "NONE      ");
static const u8 sDebugText_Weather_Clear[]       = DTR("はれ　　　", "CLOUDY    ");
static const u8 sDebugText_Weather_Clear2[]      = DTR("はれ2　　", "SUNNY     ");
static const u8 sDebugText_Weather_Rain[]        = DTR("あめ　　　", "RAIN      ");
static const u8 sDebugText_Weather_Snow[]        = DTR("ゆき　　　", "SNOW      ");
static const u8 sDebugText_Weather_Lightning[]   = DTR("かみなり　", "LIGHTNING ");
static const u8 sDebugText_Weather_Fog[]         = DTR("きり　　　", "FOG 1     ");
static const u8 sDebugText_Weather_VolcanicAsh[] = DTR("かざんばい", "ASH       ");
static const u8 sDebugText_Weather_Sandstorm[]   = DTR("すなあらし", "SANDSTORM ");
static const u8 sDebugText_Weather_Fog2[]        = DTR("きり2　　", "FOG 2     ");
static const u8 sDebugText_Weather_Underwater[]  = DTR("かいてい　", "FOG 3     ");
static const u8 sDebugText_Weather_Cloudy[]      = DTR("くもり　　", "SHADE     ");
static const u8 sDebugText_Weather_Clear3[]      = DTR("はれ3　　", "DROUGHT   ");
static const u8 sDebugText_Weather_HeavyRain[]   = DTR("おおあめ", "HEAVY RAIN");
static const u8 sDebugText_Weather_Underwater2[] = DTR("かいてい2",  "UNDERWATER");

static const u8 *const sDebugText_Weather[] =
{
    [WEATHER_NONE]              = sDebugText_Weather_None,
    [WEATHER_SUNNY_CLOUDS]      = sDebugText_Weather_Clear,
    [WEATHER_SUNNY]             = sDebugText_Weather_Clear2,
    [WEATHER_RAIN]              = sDebugText_Weather_Rain,
    [WEATHER_SNOW]              = sDebugText_Weather_Snow,
    [WEATHER_RAIN_THUNDERSTORM] = sDebugText_Weather_Lightning,
    [WEATHER_FOG_HORIZONTAL]    = sDebugText_Weather_Fog,
    [WEATHER_VOLCANIC_ASH]      = sDebugText_Weather_VolcanicAsh,
    [WEATHER_SANDSTORM]         = sDebugText_Weather_Sandstorm,
    [WEATHER_FOG_DIAGONAL]      = sDebugText_Weather_Fog2,
    [WEATHER_UNDERWATER]             = sDebugText_Weather_Underwater,
    [WEATHER_SHADE]             = sDebugText_Weather_Cloudy,
    [WEATHER_DROUGHT]           = sDebugText_Weather_Clear3,
    [WEATHER_DOWNPOUR]        = sDebugText_Weather_HeavyRain,
    [WEATHER_UNDERWATER_BUBBLES]           = sDebugText_Weather_Underwater2,
};

#endif

const u16 gFogPalette[] = INCBIN_U16("graphics/weather/fog.gbapal");

void StartWeather(void)
{
    u8 index;

    if (!FuncIsActiveTask(Task_WeatherMain))
    {
        index = AllocSpritePalette(0x1200);
        CpuCopy32(gFogPalette, &gPlttBufferUnfaded[0x100 + index * 16], 32);
        BuildColorMaps();
        gWeatherPtr->contrastColorMapSpritePalIndex = index;
        gWeatherPtr->weatherPicSpritePalIndex = AllocSpritePalette(0x1201);
        gWeatherPtr->rainSpriteCount = 0;
        gWeatherPtr->curRainSpriteIndex = 0;
        gWeatherPtr->cloudSpritesCreated = 0;
        gWeatherPtr->snowflakeSpriteCount = 0;
        gWeatherPtr->ashSpritesCreated = 0;
        gWeatherPtr->fogHSpritesCreated = 0;
        gWeatherPtr->fogDSpritesCreated = 0;
        gWeatherPtr->sandstormSpritesCreated = 0;
        gWeatherPtr->sandstormSwirlSpritesCreated = 0;
        gWeatherPtr->bubblesSpritesCreated = 0;
        gWeatherPtr->lightenedFogSpritePalsCount = 0;
        Weather_SetBlendCoeffs(16, 0);
        gWeatherPtr->currWeather = 0;
        gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
        gWeatherPtr->readyForInit = FALSE;
        gWeatherPtr->weatherChangeComplete = TRUE;
        gWeatherPtr->taskId = CreateTask(Task_WeatherInit, 80);
    }
}

void SetNextWeather(u8 weather)
{
    if (weather != WEATHER_RAIN && weather != WEATHER_RAIN_THUNDERSTORM && weather != WEATHER_DOWNPOUR)
    {
        PlayRainStoppingSoundEffect();
    }

    if (gWeatherPtr->nextWeather != weather && gWeatherPtr->currWeather == weather)
    {
        sWeatherFuncs[weather].initVars();
    }

    gWeatherPtr->weatherChangeComplete = FALSE;
    gWeatherPtr->nextWeather = weather;
    gWeatherPtr->finishStep = 0;
}

void SetCurrentAndNextWeather(u8 weather)
{
    PlayRainStoppingSoundEffect();
    gWeatherPtr->currWeather = weather;
    gWeatherPtr->nextWeather = weather;
}

void SetCurrentAndNextWeatherNoDelay(u8 weather)
{
    PlayRainStoppingSoundEffect();
    gWeatherPtr->currWeather = weather;
    gWeatherPtr->nextWeather = weather;
    gWeatherPtr->readyForInit = TRUE;
}

static void Task_WeatherInit(u8 taskId)
{
    // Waits until it's ok to initialize weather.
    // When the screen fades in, this is set to TRUE.
    if (gWeatherPtr->readyForInit)
    {
        sWeatherFuncs[gWeatherPtr->currWeather].initAll();
        gTasks[taskId].func = Task_WeatherMain;
    }
}

static void Task_WeatherMain(u8 taskId)
{
    if (gWeatherPtr->currWeather != gWeatherPtr->nextWeather)
    {
        if (!sWeatherFuncs[gWeatherPtr->currWeather].finish())
        {
            // Finished cleaning up previous weather. Now transition to next weather.
            sWeatherFuncs[gWeatherPtr->nextWeather].initVars();
            gWeatherPtr->colorMapStepCounter = 0;
            gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_CHANGING_WEATHER;
            gWeatherPtr->currWeather = gWeatherPtr->nextWeather;
            gWeatherPtr->weatherChangeComplete = TRUE;
        }
    }
    else
    {
        sWeatherFuncs[gWeatherPtr->currWeather].main();
    }

    gWeatherPalStateFuncs[gWeatherPtr->palProcessingState]();
}

static void None_Init(void)
{
    gWeatherPtr->targetColorMapIndex = 0;
    gWeatherPtr->colorMapStepDelay = 0;
}

static void None_Main(void)
{
}

static u8 None_Finish(void)
{
    return 0;
}

// Builds two tables that contain gamma shifts for palette colors.
// It's unclear why the two tables aren't declared as const arrays, since
// this function always builds the same two tables.
static void BuildColorMaps(void)
{
    u16 i;
    u8 (*colorMaps)[32];
    u16 colorVal;
    u16 curBrightness;
    u16 brightnessDelta;
    u16 colorMapIndex;
    u16 baseBrightness;
    u32 v10;
    u16 v11;
    s16 diff;

    sPaletteColorMapTypes = sBasePaletteColorMapTypes;
    for (i = 0; i <= 1; i++)
    {
        if (i == 0)
            colorMaps = gWeatherPtr->darkenedContrastColorMaps;
        else
            colorMaps = gWeatherPtr->contrastColorMaps;

        for (colorVal = 0; colorVal < 32; colorVal++)
        {
            curBrightness = colorVal << 8;
            if (i == 0)
                brightnessDelta = (colorVal << 8) / 16;
            else
                brightnessDelta = 0;
            for (colorMapIndex = 0; colorMapIndex <= 2; colorMapIndex++)
            {
                curBrightness = (curBrightness - brightnessDelta);
                colorMaps[colorMapIndex][colorVal] = curBrightness >> 8;
            }
            baseBrightness = curBrightness;
            v10 = 0x1f00 - curBrightness;
            if ((0x1f00 - curBrightness) < 0)
            {
                v10 += 0xf;
            }
            v11 = v10 >> 4;
            if (colorVal < 12)
            {
                for (; colorMapIndex < 19; colorMapIndex++)
                {
                    curBrightness += v11;
                    diff = curBrightness - baseBrightness;
                    if (diff > 0)
                        curBrightness -= (diff + ((u16)diff >> 15)) >> 1;
                    colorMaps[colorMapIndex][colorVal] = curBrightness >> 8;
                    if (colorMaps[colorMapIndex][colorVal] > 0x1f)
                        colorMaps[colorMapIndex][colorVal] = 0x1f;
                }
            }
            else
            {
                for (; colorMapIndex < 19; colorMapIndex++)
                {
                    curBrightness += v11;
                    colorMaps[colorMapIndex][colorVal] = curBrightness >> 8;
                    if (colorMaps[colorMapIndex][colorVal] > 0x1f)
                        colorMaps[colorMapIndex][colorVal] = 0x1f;
                }
            }
        }
    }
}

// When the weather is changing, it gradually updates the palettes
// towards the desired color map.
static void UpdateWeatherColorMap(void)
{
    if (gWeatherPtr->colorMapIndex == gWeatherPtr->targetColorMapIndex)
    {
        gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
    }
    else
    {
        if (++gWeatherPtr->colorMapStepCounter >= gWeatherPtr->colorMapStepDelay)
        {
            gWeatherPtr->colorMapStepCounter = 0;
            if (gWeatherPtr->colorMapIndex < gWeatherPtr->targetColorMapIndex)
                gWeatherPtr->colorMapIndex++;
            else
                gWeatherPtr->colorMapIndex--;

            ApplyColorMap(0, 32, gWeatherPtr->colorMapIndex);
        }
    }
}

static void FadeInScreenWithWeather(void)
{
    if (++gWeatherPtr->fadeInTimer > 1)
        gWeatherPtr->fadeInFirstFrame = 0;

    switch (gWeatherPtr->currWeather)
    {
    case WEATHER_RAIN:
    case WEATHER_RAIN_THUNDERSTORM:
    case WEATHER_DOWNPOUR:
    case WEATHER_SNOW:
    case WEATHER_SHADE:
        if (FadeInScreen_RainShowShade() == FALSE)
        {
            gWeatherPtr->colorMapIndex = 3;
            gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
        }
        break;
    case WEATHER_DROUGHT:
        if (FadeInScreen_Drought() == FALSE)
        {
            gWeatherPtr->colorMapIndex = -6;
            gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
        }
        break;
    case WEATHER_FOG_HORIZONTAL:
        if (FadeInScreen_FogHorizontal() == FALSE)
        {
            gWeatherPtr->colorMapIndex = 0;
            gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
        }
        break;
    case WEATHER_VOLCANIC_ASH:
    case WEATHER_SANDSTORM:
    case WEATHER_FOG_DIAGONAL:
    case WEATHER_UNDERWATER:
    default:
        if (!gPaletteFade.active)
        {
            gWeatherPtr->colorMapIndex = gWeatherPtr->targetColorMapIndex;
            gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
        }
        break;
    }
}

static bool8 FadeInScreen_RainShowShade(void)
{
    if (gWeatherPtr->fadeScreenCounter == 16)
        return FALSE;

    if (++gWeatherPtr->fadeScreenCounter >= 16)
    {
        ApplyColorMap(0, 32, 3);
        gWeatherPtr->fadeScreenCounter = 16;
        return FALSE;
    }

    ApplyColorMapWithBlend(0, 32, 3, 16 - gWeatherPtr->fadeScreenCounter, gWeatherPtr->fadeDestColor);
    return TRUE;
}

static bool8 FadeInScreen_Drought(void)
{
    if (gWeatherPtr->fadeScreenCounter == 16)
        return FALSE;

    if (++gWeatherPtr->fadeScreenCounter >= 16)
    {
        ApplyColorMap(0, 32, -6);
        gWeatherPtr->fadeScreenCounter = 16;
        return FALSE;
    }

    ApplyDroughtColorMapWithBlend(-6, 16 - gWeatherPtr->fadeScreenCounter, gWeatherPtr->fadeDestColor);
    return TRUE;
}

static bool8 FadeInScreen_FogHorizontal(void)
{
    if (gWeatherPtr->fadeScreenCounter == 16)
        return FALSE;

    gWeatherPtr->fadeScreenCounter++;
    ApplyFogBlend(16 - gWeatherPtr->fadeScreenCounter, gWeatherPtr->fadeDestColor);
    return TRUE;
}

static void DoNothing(void)
{ }

static void ApplyColorMap(u8 startPalIndex, u8 numPalettes, s8 colorMapIndex)
{
    u16 curPalIndex;
    u16 palOffset;
    u8 *colorMap;
    u16 i;

    if (colorMapIndex > 0)
    {
        colorMapIndex--;
        palOffset = startPalIndex * 16;
        numPalettes += startPalIndex;
        curPalIndex = startPalIndex;

        // Loop through the speficied palette range and apply necessary gamma shifts to the colors.
        while (curPalIndex < numPalettes)
        {
            if (sPaletteColorMapTypes[curPalIndex] == COLOR_MAP_NONE)
            {
                // No palette change.
                CpuFastCopy(gPlttBufferUnfaded + palOffset, gPlttBufferFaded + palOffset, 16 * sizeof(u16));
                palOffset += 16;
            }
            else
            {
                u8 r, g, b;

                if (sPaletteColorMapTypes[curPalIndex] == COLOR_MAP_CONTRAST || curPalIndex - 16 == gWeatherPtr->contrastColorMapSpritePalIndex)
                    colorMap = gWeatherPtr->contrastColorMaps[colorMapIndex];
                else
                    colorMap = gWeatherPtr->darkenedContrastColorMaps[colorMapIndex];

                if (curPalIndex == 16 || curPalIndex > 27)
                {
                    for (i = 0; i < 16; i++)
                    {
                        if (gPlttBufferUnfaded[palOffset] == RGB(31, 12, 11))
                        {
                            // Skip gamma shift for this specific color. (Why?)
                            palOffset++;
                        }
                        else
                        {
                            // Apply gamma shift to the original color.
                            struct RGBColor baseColor = *(struct RGBColor *)&gPlttBufferUnfaded[palOffset];
                            r = colorMap[baseColor.r];
                            g = colorMap[baseColor.g];
                            b = colorMap[baseColor.b];
                            gPlttBufferFaded[palOffset++] = (b << 10) | (g << 5) | r;
                        }
                    }
                }
                else
                {
                    for (i = 0; i < 16; i++)
                    {
                        // Apply gamma shift to the original color.
                        struct RGBColor baseColor = *(struct RGBColor *)&gPlttBufferUnfaded[palOffset];
                        r = colorMap[baseColor.r];
                        g = colorMap[baseColor.g];
                        b = colorMap[baseColor.b];
                        gPlttBufferFaded[palOffset++] = (b << 10) | (g << 5) | r;
                    }
                }
            }

            curPalIndex++;
        }
    }
    else if (colorMapIndex < 0)
    {
        // A negative gammIndex value means that the blending will come from the special Drought weather's palette tables.
        colorMapIndex = -colorMapIndex - 1;
        palOffset = startPalIndex * 16;
        numPalettes += startPalIndex;
        curPalIndex = startPalIndex;

        while (curPalIndex < numPalettes)
        {
            if (sPaletteColorMapTypes[curPalIndex] == COLOR_MAP_NONE)
            {
                // No palette change.
                CpuFastCopy(gPlttBufferUnfaded + palOffset, gPlttBufferFaded + palOffset, 16 * sizeof(u16));
                palOffset += 16;
            }
            else
            {
                if (curPalIndex == 16 || curPalIndex > 27)
                {
                    for (i = 0; i < 16; i++)
                    {
                        // Skip gamma shift for this specific color. (Why?)
                        if (gPlttBufferUnfaded[palOffset] != RGB(31, 12, 11))
                            gPlttBufferFaded[palOffset] = eDroughtPaletteData.gammaShiftColors[colorMapIndex][DROUGHT_COLOR_INDEX(gPlttBufferUnfaded[palOffset])];

                        palOffset++;
                    }
                }
                else
                {
                    for (i = 0; i < 16; i++)
                    {
                        gPlttBufferFaded[palOffset] = eDroughtPaletteData.gammaShiftColors[colorMapIndex][DROUGHT_COLOR_INDEX(gPlttBufferUnfaded[palOffset])];
                        palOffset++;
                    }
                }
            }

            curPalIndex++;
        }
    }
    else
    {
        // No palette blending.
        CpuFastCopy(gPlttBufferUnfaded + startPalIndex * 16, gPlttBufferFaded + startPalIndex * 16, numPalettes * 16 * sizeof(u16));
    }
}

static void ApplyColorMapWithBlend(u8 startPalIndex, u8 numPalettes, s8 colorMapIndex, u8 blendCoeff, u16 blendColor)
{
    u16 palOffset;
    u16 curPalIndex;
    u16 i;
    struct RGBColor color = *(struct RGBColor *)&blendColor;
    u8 rBlend = color.r;
    u8 gBlend = color.g;
    u8 bBlend = color.b;

    palOffset = startPalIndex * 16;
    numPalettes += startPalIndex;
    colorMapIndex--;
    curPalIndex = startPalIndex;

    while (curPalIndex < numPalettes)
    {
        if (sPaletteColorMapTypes[curPalIndex] == COLOR_MAP_NONE)
        {
            // No gamma shift. Simply blend the colors.
            BlendPalette(palOffset, 16, blendCoeff, blendColor);
            palOffset += 16;
        }
        else
        {
            u8 *colorMap;

            if (sPaletteColorMapTypes[curPalIndex] == COLOR_MAP_DARK_CONTRAST)
                colorMap = gWeatherPtr->darkenedContrastColorMaps[colorMapIndex];
            else
                colorMap = gWeatherPtr->contrastColorMaps[colorMapIndex];

            for (i = 0; i < 16; i++)
            {
                struct RGBColor baseColor = *(struct RGBColor *)&gPlttBufferUnfaded[palOffset];
                u8 r = colorMap[baseColor.r];
                u8 g = colorMap[baseColor.g];
                u8 b = colorMap[baseColor.b];

                // Apply gamma shift and target blend color to the original color.
                r += ((rBlend - r) * blendCoeff) >> 4;
                g += ((gBlend - g) * blendCoeff) >> 4;
                b += ((bBlend - b) * blendCoeff) >> 4;
                gPlttBufferFaded[palOffset++] = (b << 10) | (g << 5) | r;
            }
        }

        curPalIndex++;
    }
}

static void ApplyDroughtColorMapWithBlend(s8 colorMapIndex, u8 blendCoeff, u16 blendColor)
{
    struct RGBColor color;
    u8 rBlend;
    u8 gBlend;
    u8 bBlend;
    u16 curPalIndex;
    u16 palOffset;
    u16 i;

    colorMapIndex = -colorMapIndex - 1;
    color = *(struct RGBColor *)&blendColor;
    rBlend = color.r;
    gBlend = color.g;
    bBlend = color.b;
    palOffset = 0;
    for (curPalIndex = 0; curPalIndex < 32; curPalIndex++)
    {
        if (sPaletteColorMapTypes[curPalIndex] == COLOR_MAP_NONE)
        {
            // No gamma shift. Simply blend the colors.
            BlendPalette(palOffset, 16, blendCoeff, blendColor);
            palOffset += 16;
        }
        else
        {
            for (i = 0; i < 16; i++)
            {
                u32 offset;
                struct RGBColor color1;
                struct RGBColor color2;
                u8 r1, g1, b1;
                u8 r2, g2, b2;

                color1 = *(struct RGBColor *)&gPlttBufferUnfaded[palOffset];
                r1 = color1.r;
                g1 = color1.g;
                b1 = color1.b;

                offset = ((b1 & 0x1E) << 7) | ((g1 & 0x1E) << 3) | ((r1 & 0x1E) >> 1);
                color2 = *(struct RGBColor *)&eDroughtPaletteData.gammaShiftColors[colorMapIndex][offset];
                r2 = color2.r;
                g2 = color2.g;
                b2 = color2.b;

                r2 += ((rBlend - r2) * blendCoeff) >> 4;
                g2 += ((gBlend - g2) * blendCoeff) >> 4;
                b2 += ((bBlend - b2) * blendCoeff) >> 4;

                gPlttBufferFaded[palOffset++] = (b2 << 10) | (g2 << 5) | r2;
            }
        }
    }
}

static void ApplyFogBlend(u8 blendCoeff, u16 blendColor)
{
    struct RGBColor color;
    u8 rBlend;
    u8 gBlend;
    u8 bBlend;
    u16 curPalIndex;

    BlendPalette(0, 256, blendCoeff, blendColor);
    color = *(struct RGBColor *)&blendColor;
    rBlend = color.r;
    gBlend = color.g;
    bBlend = color.b;

    for (curPalIndex = 16; curPalIndex < 32; curPalIndex++)
    {
        if (LightenSpritePaletteInFog(curPalIndex))
        {
            u16 palEnd = (curPalIndex + 1) * 16;
            u16 palOffset = curPalIndex * 16;

            while (palOffset < palEnd)
            {
                struct RGBColor color = *(struct RGBColor *)&gPlttBufferUnfaded[palOffset];
                u8 r = color.r;
                u8 g = color.g;
                u8 b = color.b;

                r += ((28 - r) * 3) >> 2;
                g += ((31 - g) * 3) >> 2;
                b += ((28 - b) * 3) >> 2;

                r += ((rBlend - r) * blendCoeff) >> 4;
                g += ((gBlend - g) * blendCoeff) >> 4;
                b += ((bBlend - b) * blendCoeff) >> 4;

                gPlttBufferFaded[palOffset] = (b << 10) | (g << 5) | r;
                palOffset++;
            }
        }
        else
        {
            BlendPalette(curPalIndex * 16, 16, blendCoeff, blendColor);
        }
    }
}

static void MarkFogSpritePalToLighten(u8 paletteIndex)
{
    if (gWeatherPtr->lightenedFogSpritePalsCount < 6)
    {
        gWeatherPtr->lightenedFogSpritePals[gWeatherPtr->lightenedFogSpritePalsCount] = paletteIndex;
        gWeatherPtr->lightenedFogSpritePalsCount++;
    }
}

static bool8 LightenSpritePaletteInFog(u8 paletteIndex)
{
    u16 i;

    for (i = 0; i < gWeatherPtr->lightenedFogSpritePalsCount; i++)
    {
        if (gWeatherPtr->lightenedFogSpritePals[i] == paletteIndex)
            return TRUE;
    }

    return FALSE;
}

void ApplyWeatherColorMapIfIdle(s8 colorMapIndex)
{
    if (gWeatherPtr->palProcessingState == WEATHER_PAL_STATE_IDLE)
    {
        ApplyColorMap(0, 32, colorMapIndex);
        gWeatherPtr->colorMapIndex = colorMapIndex;
    }
}

void ApplyWeatherColorMapIfIdle_Gradual(u8 colorMapIndex, u8 targetColorMapIndex, u8 colorMapStepDelay)
{
    if (gWeatherPtr->palProcessingState == WEATHER_PAL_STATE_IDLE)
    {
        gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_CHANGING_WEATHER;
        gWeatherPtr->colorMapIndex = colorMapIndex;
        gWeatherPtr->targetColorMapIndex = targetColorMapIndex;
        gWeatherPtr->colorMapStepCounter = 0;
        gWeatherPtr->colorMapStepDelay = colorMapStepDelay;
        ApplyWeatherColorMapIfIdle(colorMapIndex);
    }
}

void FadeScreen(u8 mode, u8 delay)
{
    u32 fadeColor;
    bool8 fadeOut;
    bool8 useWeatherPal;

    switch (mode)
    {
    case FADE_FROM_BLACK:
        fadeColor = 0;
        fadeOut = FALSE;
        break;
    case FADE_FROM_WHITE:
        fadeColor = 0xFFFF;
        fadeOut = FALSE;
        break;
    case FADE_TO_BLACK:
        fadeColor = 0;
        fadeOut = TRUE;
        break;
    case FADE_TO_WHITE:
        fadeColor = 0xFFFF;
        fadeOut = TRUE;
        break;
    default:
        return;
    }

    switch (gWeatherPtr->currWeather)
    {
    case WEATHER_RAIN:
    case WEATHER_RAIN_THUNDERSTORM:
    case WEATHER_DOWNPOUR:
    case WEATHER_SNOW:
    case WEATHER_FOG_HORIZONTAL:
    case WEATHER_SHADE:
    case WEATHER_DROUGHT:
        useWeatherPal = TRUE;
        break;
    default:
        useWeatherPal = FALSE;
        break;
    }

    if (fadeOut)
    {
        if (useWeatherPal)
            CpuFastCopy(gPlttBufferFaded, gPlttBufferUnfaded, 0x400);

        BeginNormalPaletteFade(0xFFFFFFFF, delay, 0, 16, fadeColor);
        gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_SCREEN_FADING_OUT;
    }
    else
    {
        gWeatherPtr->fadeDestColor = fadeColor;
        if (useWeatherPal)
            gWeatherPtr->fadeScreenCounter = 0;
        else
            BeginNormalPaletteFade(0xFFFFFFFF, delay, 16, 0, fadeColor);

        gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_SCREEN_FADING_IN;
        gWeatherPtr->fadeInFirstFrame = 1;
        gWeatherPtr->fadeInTimer = 0;
        Weather_SetBlendCoeffs(gWeatherPtr->currBlendEVA, gWeatherPtr->currBlendEVB);
        gWeatherPtr->readyForInit = TRUE;
    }
}

bool8 IsWeatherNotFadingIn(void)
{
    return (gWeatherPtr->palProcessingState != WEATHER_PAL_STATE_SCREEN_FADING_IN);
}

void UpdateSpritePaletteWithWeather(u8 spritePaletteIndex)
{
    u16 paletteIndex = 16 + spritePaletteIndex;
    u16 i;

    switch (gWeatherPtr->palProcessingState)
    {
    case WEATHER_PAL_STATE_SCREEN_FADING_IN:
        if (gWeatherPtr->fadeInFirstFrame != 0)
        {
            if (gWeatherPtr->currWeather == WEATHER_FOG_HORIZONTAL)
                MarkFogSpritePalToLighten(paletteIndex);
            paletteIndex *= 16;
            for (i = 0; i < 16; i++)
                gPlttBufferFaded[paletteIndex + i] = gWeatherPtr->fadeDestColor;
        }
        break;
    case WEATHER_PAL_STATE_SCREEN_FADING_OUT:
        paletteIndex *= 16;
        CpuFastCopy(gPlttBufferFaded + paletteIndex, gPlttBufferUnfaded + paletteIndex, 32);
        BlendPalette(paletteIndex, 16, gPaletteFade.y, gPaletteFade.blendColor);
        break;
    // WEATHER_PAL_STATE_CHANGING_WEATHER
    // WEATHER_PAL_STATE_CHANGING_IDLE
    default:
        if (gWeatherPtr->currWeather != WEATHER_FOG_HORIZONTAL)
        {
            ApplyColorMap(paletteIndex, 1, gWeatherPtr->colorMapIndex);
        }
        else
        {
            paletteIndex *= 16;
            BlendPalette(paletteIndex, 16, 12, RGB(28, 31, 28));
        }
        break;
    }
}

void ApplyWeatherColorMapToPal(u8 paletteIndex)
{
    ApplyColorMap(paletteIndex, 1, gWeatherPtr->colorMapIndex);
}

static bool8 UNUSED IsFirstFrameOfWeatherFadeIn(void)
{
    if (gWeatherPtr->palProcessingState == WEATHER_PAL_STATE_SCREEN_FADING_IN)
        return gWeatherPtr->fadeInFirstFrame;
    else
        return 0;
}

void LoadCustomWeatherSpritePalette(const u16 *palette)
{
    LoadPalette(palette, 0x100 + gWeatherPtr->weatherPicSpritePalIndex * 16, 32);
    UpdateSpritePaletteWithWeather(gWeatherPtr->weatherPicSpritePalIndex);
}

static void LoadDroughtWeatherPalette(u8 *palsIndex, u8 *palsOffset)
{
    u8 gammaIndex = *palsIndex;
    u16 i;

    if (gammaIndex < 7)
    {
        gammaIndex--;
        LZ77UnCompWram(sCompressedDroughtPalettes[gammaIndex], eDroughtPaletteData.gammaShiftColors[gammaIndex]);
        if (gammaIndex == 0)
        {
            eDroughtPaletteData.gammaShiftColors[gammaIndex][0] = RGB(1, 1, 1);
            for (i = 1; i < 0x1000; i++)
                eDroughtPaletteData.gammaShiftColors[gammaIndex][i] += eDroughtPaletteData.gammaShiftColors[gammaIndex][i - 1];
        }
        else
        {
            for (i = 0; i < 0x1000; i++)
                eDroughtPaletteData.gammaShiftColors[gammaIndex][i] += eDroughtPaletteData.gammaShiftColors[gammaIndex - 1][i];
        }
        if (++(*palsIndex) == 7)
        {
            *palsIndex = 32;
            *palsOffset = 32;
        }
    }
}

void ResetDroughtWeatherPaletteLoading(void)
{
    gWeatherPtr->loadDroughtPalsIndex = 1;
    gWeatherPtr->loadDroughtPalsOffset = 1;
}

bool8 LoadDroughtWeatherPalettes(void)
{
    if (gWeatherPtr->loadDroughtPalsIndex < 32)
    {
        LoadDroughtWeatherPalette(&gWeatherPtr->loadDroughtPalsIndex, &gWeatherPtr->loadDroughtPalsOffset);
        if (gWeatherPtr->loadDroughtPalsIndex < 32)
            return TRUE;
    }
    return FALSE;
}

static void SetDroughtColorMap(s8 colorMapIndex)
{
    ApplyWeatherColorMapIfIdle(-colorMapIndex - 1);
}

void DroughtStateInit(void)
{
    gWeatherPtr->droughtBrightnessStage = 0;
    gWeatherPtr->droughtTimer = 0;
    gWeatherPtr->droughtState = 0;
    gWeatherPtr->droughtLastBrightnessStage = 0;
    gUnknown_0202FF58 = 5;
}

void DroughtStateRun(void)
{
    switch (gWeatherPtr->droughtState)
    {
    case 0:
        if (++gWeatherPtr->droughtTimer > gUnknown_0202FF58)
        {
            gWeatherPtr->droughtTimer = 0;
            SetDroughtColorMap(gWeatherPtr->droughtBrightnessStage++);
            if (gWeatherPtr->droughtBrightnessStage > 5)
            {
                gWeatherPtr->droughtLastBrightnessStage = gWeatherPtr->droughtBrightnessStage;
                gWeatherPtr->droughtState = 1;
                gWeatherPtr->droughtTimer = 0x3C;
            }
        }
        break;
    case 1:
        gWeatherPtr->droughtTimer = (gWeatherPtr->droughtTimer + 3) & 0x7F;
        gWeatherPtr->droughtBrightnessStage = ((gSineTable[gWeatherPtr->droughtTimer] - 1) >> 6) + 2;
        if (gWeatherPtr->droughtBrightnessStage != gWeatherPtr->droughtLastBrightnessStage)
            SetDroughtColorMap(gWeatherPtr->droughtBrightnessStage);
        gWeatherPtr->droughtLastBrightnessStage = gWeatherPtr->droughtBrightnessStage;
        break;
    case 2:
        if (++gWeatherPtr->droughtTimer > gUnknown_0202FF58)
        {
            gWeatherPtr->droughtTimer = 0;
            SetDroughtColorMap(--gWeatherPtr->droughtBrightnessStage);
            if (gWeatherPtr->droughtBrightnessStage == 3)
                gWeatherPtr->droughtState = 0;
        }
        break;
    }
}

void Weather_SetBlendCoeffs(u8 eva, u8 evb)
{
    gWeatherPtr->currBlendEVA = eva;
    gWeatherPtr->currBlendEVB = evb;
    gWeatherPtr->targetBlendEVA = eva;
    gWeatherPtr->targetBlendEVB = evb;
    REG_BLDALPHA = BLDALPHA_BLEND(eva, evb);
}

void Weather_SetTargetBlendCoeffs(u8 eva, u8 evb, int delay)
{
    gWeatherPtr->targetBlendEVA = eva;
    gWeatherPtr->targetBlendEVB = evb;
    gWeatherPtr->blendDelay = delay;
    gWeatherPtr->blendFrameCounter = 0;
    gWeatherPtr->blendUpdateCounter = 0;
}

bool8 Weather_UpdateBlend(void)
{
    if (gWeatherPtr->currBlendEVA == gWeatherPtr->targetBlendEVA
     && gWeatherPtr->currBlendEVB == gWeatherPtr->targetBlendEVB)
        return TRUE;

    if (++gWeatherPtr->blendFrameCounter > gWeatherPtr->blendDelay)
    {
        gWeatherPtr->blendFrameCounter = 0;
        gWeatherPtr->blendUpdateCounter++;

        // Update currBlendEVA and currBlendEVB on alternate frames
        if (gWeatherPtr->blendUpdateCounter & 1)
        {
            if (gWeatherPtr->currBlendEVA < gWeatherPtr->targetBlendEVA)
                gWeatherPtr->currBlendEVA++;
            else if (gWeatherPtr->currBlendEVA > gWeatherPtr->targetBlendEVA)
                gWeatherPtr->currBlendEVA--;
        }
        else
        {
            if (gWeatherPtr->currBlendEVB < gWeatherPtr->targetBlendEVB)
                gWeatherPtr->currBlendEVB++;
            else if (gWeatherPtr->currBlendEVB > gWeatherPtr->targetBlendEVB)
                gWeatherPtr->currBlendEVB--;
        }
    }

    REG_BLDALPHA = BLDALPHA_BLEND(gWeatherPtr->currBlendEVA, gWeatherPtr->currBlendEVB);

    if (gWeatherPtr->currBlendEVA == gWeatherPtr->targetBlendEVA
     && gWeatherPtr->currBlendEVB == gWeatherPtr->targetBlendEVB)
        return TRUE;

    return FALSE;
}

static void UNUSED SetFieldWeather(u8 weather)
{
    switch (weather)
    {
    case 1:
        SetWeather(WEATHER_SUNNY_CLOUDS);
        break;
    case 2:
        SetWeather(WEATHER_SUNNY);
        break;
    case 3:
        SetWeather(WEATHER_RAIN);
        break;
    case 4:
        SetWeather(WEATHER_SNOW);
        break;
    case 5:
        SetWeather(WEATHER_RAIN_THUNDERSTORM);
        break;
    case 6:
        SetWeather(WEATHER_FOG_HORIZONTAL);
        break;
    case 7:
        SetWeather(WEATHER_FOG_DIAGONAL);
        break;
    case 8:
        SetWeather(WEATHER_VOLCANIC_ASH);
        break;
    case 9:
        SetWeather(WEATHER_SANDSTORM);
        break;
    case 10:
        SetWeather(WEATHER_SHADE);
        break;
    }
}

u8 GetCurrentWeather(void)
{
    return gWeatherPtr->currWeather;
}

void SetRainStrengthFromSoundEffect(u16 soundEffect)
{
    if (gWeatherPtr->palProcessingState != WEATHER_PAL_STATE_SCREEN_FADING_OUT)
    {
        switch (soundEffect)
        {
        case SE_RAIN:
            gWeatherPtr->rainStrength = 0;
            break;
        case SE_DOWNPOUR:
            gWeatherPtr->rainStrength = 1;
            break;
        case SE_THUNDERSTORM:
            gWeatherPtr->rainStrength = 2;
            break;
        default:
            return;
        }

        PlaySE(soundEffect);
    }
}

void PlayRainStoppingSoundEffect(void)
{
    if (IsSpecialSEPlaying())
    {
        switch (gWeatherPtr->rainStrength)
        {
        case 0:
            PlaySE(SE_RAIN_STOP);
            break;
        case 1:
            PlaySE(SE_DOWNPOUR_STOP);
            break;
        case 2:
        default:
            PlaySE(SE_THUNDERSTORM_STOP);
            break;
        }
    }
}

u8 IsWeatherChangeComplete(void)
{
    return gWeatherPtr->weatherChangeComplete;
}

void SetWeatherScreenFadeOut(void)
{
    gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_SCREEN_FADING_OUT;
}

void SetWeatherPalStateIdle(void)
{
    gWeatherPtr->palProcessingState = WEATHER_PAL_STATE_IDLE;
}

void PreservePaletteInWeather(u8 preservedPalIndex)
{
    CpuCopy16(sBasePaletteColorMapTypes, sFieldEffectPaletteColorMapTypes, 32);
    sFieldEffectPaletteColorMapTypes[preservedPalIndex] = COLOR_MAP_NONE;
    sPaletteColorMapTypes = sFieldEffectPaletteColorMapTypes;
}

void ResetPreservedPalettesInWeather(void)
{
    sPaletteColorMapTypes = sBasePaletteColorMapTypes;
}

#if DEBUG

EWRAM_DATA static u8 sSelectedDebugWeather = 0;

bool8 debug_sub_8085564(void)
{
    bool8 changed = FALSE;

    if (JOY_NEW(R_BUTTON))
    {
        sSelectedDebugWeather++;
        if (sSelectedDebugWeather == 15)
            sSelectedDebugWeather = 0;
        changed = TRUE;
    }
    if (JOY_NEW(L_BUTTON))
    {
        if (sSelectedDebugWeather != 0)
            sSelectedDebugWeather--;
        else
            sSelectedDebugWeather = 14;
        changed = TRUE;
    }

    if (changed)
    {
        Menu_BlankWindowRect(22, 1, 28, 2);
        Menu_PrintText(sDebugText_Weather[sSelectedDebugWeather], 23, 1);
    }
    
    if (JOY_NEW(A_BUTTON))
    {
        SetNextWeather(sSelectedDebugWeather);
        CloseMenu();
        return TRUE;
    }
    
    return FALSE;
}

bool8 debug_sub_808560C(void)
{
    sSelectedDebugWeather = gWeather.currWeather;
    Menu_EraseScreen();
    Menu_BlankWindowRect(22, 1, 28, 2);
    Menu_PrintText(sDebugText_Weather[sSelectedDebugWeather], 23, 1);
    gMenuCallback = debug_sub_8085564;
    return FALSE;
}

#endif
