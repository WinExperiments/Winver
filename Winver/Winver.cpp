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
Element* tabPill;
TouchButton *tab1, *tab2;
Element *page1, *page2;

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

void SwitchPage(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        if (elem == tab1) {
            page2->SetLayoutPos(4);
            page2->SetAlpha(0);
            page1->SetLayoutPos(4);
            page1->SetAlpha(255);
            tabPill->SetX(40);
        }
        if (elem == tab2) {
            page1->SetLayoutPos(4);
            page1->SetAlpha(0);
            page2->SetLayoutPos(4);
            page2->SetAlpha(255);
            tabPill->SetX(152);
        }
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
    LPCWSTR id2 = MAKEINTRESOURCE(108);
    HBITMAP bmp = (HBITMAP)LoadImage(inst, id, IMAGE_BITMAP, 32, 32, 0);
    HBITMAP bmp2 = (HBITMAP)LoadImage(inst, id2, IMAGE_BITMAP, 16, 3, 0);
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

void SetTheme() {
    StyleSheet* sheet{};
    Value* sheetStorage{};
    parser->GetSheet((UCString)sheetName, &sheetStorage);
    sheet = sheetStorage->GetStyleSheet();
    pMain->SetSheet(sheet);
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

    NativeHWNDHost::Create((UCString)L"About Windows", NULL, LoadIconW(inst, MAKEINTRESOURCE(1024)), CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, NULL, WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, 0, &wnd);
    DUIXmlParser::Create(&parser, NULL, NULL, NULL, NULL);
    parser->SetXMLFromResource(IDR_UIFILE2, hInstance, hInstance);
    HWNDElement::Create(wnd->GetHWND(), true, NULL, NULL, &key, (Element**)&parent);
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