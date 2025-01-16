// DUIAppTemplate.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DirectUI/DirectUI.h"
#include "Winver.h"
#include <string>
#include "resource.h"
#include <propkey.h>
#include "strsafe.h"
#include "StyleModifier.h"
#include "BitmapHelper.h"
#include <WinUser.h>
#include <thread>
#include <chrono>

using namespace DirectUI;
using namespace std;

NativeHWNDHost* wnd;
HWNDElement* parent;
DUIXmlParser* parser;
Element* pMain;
unsigned long key = 0;

TouchButton* samplecolor;
Element* tabPill;
TouchButton *tab1, *tab2;
Element *page1, *page2;

HRESULT err;
HINSTANCE inst = GetModuleHandle(NULL);
WNDPROC WndProc;
HWND hWnd;
HANDLE listenerHandle;
HANDLE hMutex;
constexpr LPCWSTR szWindowClass = L"WINVER";

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DPICHANGED) {
        UpdateScale();
        ModifyStyle();
    }
    if (uMsg == WM_SETTINGCHANGE) {
        UpdateModeInfo();
        ModifyStyle();
        SetTheme();
    }
    return CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam);
}

struct EventListener : public IElementListener {

    void (*f)(Element*, Event*);

    EventListener(void (*func)(Element*, Event*)) {
        f = func; 
    }

    void OnListenerAttach(Element* elem) override { }
    void OnListenerDetach(Element* elem) override { }
    bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
        return true;
    }
    void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override {

    }
    void OnListenedEvent(Element* elem, struct Event* iev) override {
        f(elem, iev);
    }
    void OnListenedInput(Element* elem, struct InputEvent* ev) override { 
    }
};

int dpi, dpiOld = 1, pageIndex = 0;
bool isDpiPreviouslyChanged;
void InitialUpdateScale() {
    HDC screen = GetDC(0);
    dpi = static_cast<int>(GetDeviceCaps(screen, LOGPIXELSX));
    ReleaseDC(0, screen);
}

void UpdateScale() {
    static int windowsThemeX = (GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) * 2) * 2;
    static int windowsThemeY = windowsThemeX + GetSystemMetrics(SM_CYCAPTION);
    dpiOld = dpi;
    dpi = GetDpiForWindow(hWnd);
    tabPill->SetX((40 + 112 * pageIndex) * dpi / 96);
    UpdateFontSize();
    SetWindowPos(hWnd, NULL, NULL, NULL, (600 + windowsThemeX) * dpi / 96, (600 + windowsThemeY) * dpi / 96, SWP_NOMOVE);
    isDpiPreviouslyChanged = true;
}

void testEventListener(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        exit(0);
    }
}

void SwitchPage(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        if (elem == tab1) {
            pageIndex = 0;
            page2->SetLayoutPos(4);
            page2->SetAlpha(0);
            page1->SetLayoutPos(4);
            page1->SetAlpha(255);
        }
        if (elem == tab2) {
            pageIndex = 1;
            page1->SetLayoutPos(4);
            page1->SetAlpha(0);
            page2->SetLayoutPos(4);
            page2->SetAlpha(255);
        }
        tabPill->SetX((40 + 112 * pageIndex) * dpi / 96);
    }
}


unsigned long MessageListener(LPVOID lpParam) {
    hMutex = CreateMutex(NULL, TRUE, szWindowClass);
    if (!hMutex || ERROR_ALREADY_EXISTS == GetLastError())
    {
        return 1;
    }

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = SubclassWindowProc;
    wcex.hInstance = inst;
    wcex.lpszClassName = szWindowClass;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
}

void StartListener() {
    DWORD listener;
    listenerHandle = CreateThread(0, 0, MessageListener, NULL, 0, &listener);
} 

int GetScaleInterval() {
    if (dpi >= 384) return 7;
    if (dpi >= 288) return 6;
    if (dpi >= 240) return 5;
    if (dpi >= 192) return 4;
    if (dpi >= 144) return 3;
    if (dpi >= 120) return 2;
    if (dpi >= 96) return 1;
}

double GetScaleSizingInterval() {
    if (dpi >= 384) return 4.0;
    if (dpi >= 288) return 3.0;
    if (dpi >= 240) return 2.5;
    if (dpi >= 192) return 2.0;
    if (dpi >= 144) return 1.5;
    if (dpi >= 120) return 1.25;
    if (dpi >= 96) return 1.0;
}

int GetScaleSizingInterval2() {
    if (dpi >= 384) return 30;
    if (dpi >= 288) return 22;
    if (dpi >= 240) return 14;
    if (dpi >= 192) return 10;
    if (dpi >= 144) return 6;
    if (dpi >= 120) return 2;
    if (dpi >= 96) return 0;
}

void ModifyStyle()
{
    int idNum = 100 + GetScaleInterval();
    int idNum2 = 107 + GetScaleInterval();
    LPCWSTR id = MAKEINTRESOURCE(idNum);
    LPCWSTR id2 = MAKEINTRESOURCE(idNum2);
    HBITMAP bmp = (HBITMAP)LoadImage(inst, id, IMAGE_BITMAP, 11 + GetScaleSizingInterval2(), 11 + GetScaleSizingInterval2(), 0);
    HBITMAP bmp2 = (HBITMAP)LoadImage(inst, id2, IMAGE_BITMAP, 16 * GetScaleSizingInterval(), ceil(3 * GetScaleSizingInterval()), 0);
    IterateBitmap(bmp, StandardBitmapPixelHandler);
    IterateBitmap(bmp2, StandardBitmapPixelHandler);
    Value* bitmap = DirectUI::Value::CreateGraphic(bmp, 7, 0xffffffff, false, false, false);
    Value* bitmap2 = DirectUI::Value::CreateGraphic(bmp2, 2, 0xffffffff, false, false, false);
    if (samplecolor != NULL) {
        samplecolor->SetValue(Element::BackgroundProp, 1, bitmap);
        samplecolor->SetForegroundColor(btnforeground);
    }
    if (tabPill != NULL) {
        tabPill->SetValue(Element::ContentProp, 1, bitmap2);
    }
    bitmap->Release();
    bitmap2->Release();
}

void UpdateFontSize() {
    static WCHAR buffer[32];
    if (!isDpiPreviouslyChanged) {
        LoadStringW(inst, IDS_FONTSIZE, buffer, 32);
    }
    WCHAR fontSizeStr[8];
    int charCount = 0, charCount2, intCount = 0;
    while (buffer[charCount] > 57) {
        charCount++;
    }
    charCount2 = charCount;
    while (buffer[charCount2] <= 57 && buffer[charCount2] >= 48) {
        charCount2++;
        intCount++;
    }
    wcsncpy_s(fontSizeStr, 8, &buffer[charCount], intCount);
    double fontSize = _wtof(fontSizeStr);
    static double fontSizeBuffer;
    fontSizeBuffer = isDpiPreviouslyChanged ? fontSizeBuffer * dpi / dpiOld : fontSize * dpi / dpiOld;
    buffer[charCount] = '\0';
    wcscat_s(buffer, to_wstring((int)fontSizeBuffer).c_str());
    wcscat_s(buffer, L"%");
    samplecolor->SetFont((UCString)buffer);
    tab1->SetFont((UCString)buffer);
    tab2->SetFont((UCString)buffer);
    page1->SetFont((UCString)buffer);
    page2->SetFont((UCString)buffer);
}

void SetTheme() {
    StyleSheet* sheet = pMain->GetSheet();
    Value* sheetStorage = DirectUI::Value::CreateStyleSheet(sheet);
    parser->GetSheet((UCString)sheetName, &sheetStorage);
    pMain->SetValue(Element::SheetProp, 1, sheetStorage);
    sheetStorage->Release();
    return;
}

Element* regElem(const wchar_t* elemName) {
    Element* result = (Element*)pMain->FindDescendent(StrToID((UCString)elemName));
    return result;
}
RichText* regRichText(const wchar_t* elemName) {
    RichText* result = (RichText*)pMain->FindDescendent(StrToID((UCString)elemName));
    return result;
}
Button* regBtn(const wchar_t* btnName) {
    Button* result = (Button*)pMain->FindDescendent(StrToID((UCString)btnName));
    return result;
}
TouchButton* regTouchBtn(const wchar_t* btnName) {
    TouchButton* result = (TouchButton*)pMain->FindDescendent(StrToID((UCString)btnName));
    return result;
}
Edit* regEdit(const wchar_t* editName) {
    Edit* result = (Edit*)pMain->FindDescendent(StrToID((UCString)editName));
    return result;
}
void assignFn(Element* btnName, void(*fnName)(Element* elem, Event* iev)) {
    btnName->AddListener(new EventListener(fnName));
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    InitProcessPriv(14, NULL, 0, true);
    InitThread(2);
    RegisterAllControls();

    InitialUpdateScale();
    int windowsThemeX = (GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) * 2) * 2;
    int windowsThemeY = windowsThemeX + GetSystemMetrics(SM_CYCAPTION);
    NativeHWNDHost::Create((UCString)L"About Windows", NULL, LoadIconW(inst, MAKEINTRESOURCE(1024)), CW_USEDEFAULT, CW_USEDEFAULT, (600 + windowsThemeX) * dpi / 96, (600 + windowsThemeY) * dpi / 96, NULL, WS_SYSMENU, 0, &wnd);
    DUIXmlParser::Create(&parser, NULL, NULL, NULL, NULL);
    parser->SetXMLFromResource(IDR_UIFILE2, hInstance, hInstance);
    HWNDElement::Create(wnd->GetHWND(), true, NULL, NULL, &key, (Element**)&parent);
    hWnd = wnd->GetHWND();
    WndProc = (WNDPROC)SetWindowLongPtr(wnd->GetHWND(), GWLP_WNDPROC, (LONG_PTR)SubclassWindowProc);

    parser->CreateElement((UCString)L"main", parent, NULL, NULL, &pMain);
    pMain->SetVisible(true);
    pMain->EndDefer(key);
    wnd->Host(pMain);
    
    samplecolor = regTouchBtn(L"samplecolor");
    tabPill = regElem(L"tabPill");
    tab1 = regTouchBtn(L"tab1");
    tab2 = regTouchBtn(L"tab2");
    page1 = regElem(L"page1");
    page2 = regElem(L"page2");

    assignFn(samplecolor, testEventListener);
    assignFn(tab1, SwitchPage);
    assignFn(tab2, SwitchPage);

    UpdateModeInfo();
    ModifyStyle();
    SetTheme();

    wnd->ShowWindow(SW_SHOW);
    StartMessagePump();
    UnInitProcessPriv(0);

    return 0;
}