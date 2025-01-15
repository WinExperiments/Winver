#pragma once
#include "framework.h"

extern int btnforeground;
extern bool theme;
extern const wchar_t* sheetName;

void UpdateScale();
void UpdateModeInfo();
void ModifyStyle();
void UpdateFontSize();
void SetTheme();

void StandardBitmapPixelHandler(int& r, int& g, int& b, int& a);