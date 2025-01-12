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

using namespace DirectUI;
NativeHWNDHost* wnd;
HWNDElement* parent;
DUIXmlParser* parser;
Element* pMain;


unsigned long key = 0;

TouchButton* samplecolor;

HRESULT err;
HINSTANCE inst = GetModuleHandle(NULL);
WNDPROC WndProc;
HANDLE colorListenerHandle;
HANDLE hMutex;
constexpr LPCWSTR szWindowClass = L"WINVER";

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_SETTINGCHANGE) {
        UpdateModeInfo();
        ModifyStyle();
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

void testEventListener(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        exit(0);
    }
}


unsigned long UpdateColor(LPVOID lpParam) {
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
    DWORD colorListener;
    colorListenerHandle = CreateThread(0, 0, UpdateColor, NULL, 0, &colorListener);
}

void ModifyStyle()
{
    LPCWSTR id = MAKEINTRESOURCE(101);
    HBITMAP bmp = (HBITMAP)LoadImage(inst, id, IMAGE_BITMAP, 32, 32, 0);
    if (bmp == NULL)
    {
        return;
    }
    IterateBitmap(bmp, StandardBitmapPixelHandler);
    Value* test;
    Value* bitmap = DirectUI::Value::CreateGraphic(bmp, 7, 0xffffffff, false, false, false);
    if (samplecolor != NULL) {
        samplecolor->SetValue(Element::BackgroundProp, 1, bitmap);
        samplecolor->SetForegroundColor(btnforeground);
        pMain->SetBackgroundColor(wndbackground);
    }
    bitmap->Release();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    InitProcessPriv(14, NULL, 0, true);
    InitThread(2);
    RegisterAllControls();

    NativeHWNDHost::Create((UCString)L"colors demo", NULL, NULL, CW_USEDEFAULT, CW_USEDEFAULT, 320, 200, NULL, WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, 0, &wnd);
    DUIXmlParser::Create(&parser, NULL, NULL, NULL, NULL);
    parser->SetXMLFromResource(IDR_UIFILE2, hInstance, hInstance);
    HWNDElement::Create(wnd->GetHWND(), true, NULL, NULL, &key, (Element**)&parent);
    WndProc = (WNDPROC)SetWindowLongPtr(wnd->GetHWND(), GWLP_WNDPROC, (LONG_PTR)SubclassWindowProc);

    parser->CreateElement((UCString)L"main", parent, NULL, NULL, &pMain);
    pMain->SetVisible(true);
    pMain->EndDefer(key);
    wnd->Host(pMain);

    samplecolor = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"samplecolor"));

    samplecolor->AddListener(new EventListener(testEventListener));

    UpdateModeInfo();
    ModifyStyle();

    wnd->ShowWindow(SW_SHOW);
    StartMessagePump();
    UnInitProcessPriv(0);

    return 0;
}