#include "StyleModifier.h"
#include "BitmapHelper.h"
#include "ColorHelper.h"
#include "ImmersiveColor.h"

COLORREF ImmersiveColor;
int wndbackground;
int btnforeground;

void UpdateModeInfo() {
    int i = 0;
    bool b = 0;
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
    if (i == 1) b = 1;
    else b = 0;
    ImmersiveColor = b ? CImmersiveColor::GetColor(IMCLR_SystemAccentDark1) : CImmersiveColor::GetColor(IMCLR_SystemAccentLight2);
    wndbackground = b ? 4294967295 : 4280295456;
    btnforeground = b ? 16777215 : 0;
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