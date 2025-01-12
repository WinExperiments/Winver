#include "StyleModifier.h"
#include "BitmapHelper.h"
#include "ColorHelper.h"
#include "ImmersiveColor.h"

COLORREF ImmersiveColor;
int btnforeground;
bool theme;
const wchar_t* sheetName;

void UpdateModeInfo() {
    int i = 0;
    LPCWSTR path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    HKEY hKey;
    DWORD lResult = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_READ, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        DWORD dwSize = NULL;
        lResult = RegGetValue(hKey, NULL, L"AppsUseLightTheme", RRF_RT_DWORD, NULL, NULL, &dwSize);
        if (lResult == ERROR_SUCCESS && dwSize != NULL)
        {
            DWORD* dwValue = (DWORD*)malloc(dwSize);
            lResult = RegGetValue(hKey, NULL, L"AppsUseLightTheme", RRF_RT_DWORD, NULL, dwValue, &dwSize);
            i = *dwValue;
            free(dwValue);
        }
        RegCloseKey(hKey);
    }
    if (i == 1) theme = 1;
    else theme = 0;
    ImmersiveColor = theme ? CImmersiveColor::GetColor(IMCLR_SystemAccentDark1) : CImmersiveColor::GetColor(IMCLR_SystemAccentLight2);
    btnforeground = theme ? 16777215 : 0;
    sheetName = theme ? L"w" : L"wd";
}

rgb_t ImmersiveToRGB(COLORREF immersivecolor) {
	rgb_t result;
	result.r = (int)(immersivecolor % 16777216);
	result.g = (int)((immersivecolor / 256) % 65536);
	result.b = (int)((immersivecolor / 65536) % 256);
	return result;
}


void StandardBitmapPixelHandler(int& r, int& g, int& b, int& a)
{
	rgb_t rgbVal = ImmersiveToRGB(ImmersiveColor);
	rgbVal.r = rgbVal.r / (a / 255.0);
	rgbVal.g = rgbVal.g / (a / 255.0);
	rgbVal.b = rgbVal.b / (a / 255.0);


	rgbVal.r = rgbVal.r * (a / 255.0);
	rgbVal.g = rgbVal.g * (a / 255.0);
	rgbVal.b = rgbVal.b * (a / 255.0);

	r = rgbVal.r;
	g = rgbVal.g;
	b = rgbVal.b;
}