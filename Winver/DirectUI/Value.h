#pragma once

interface ISharedBitmap;

enum DynamicScaleValue : _int32 {
    DSV_None = 0x0,
    DSV_Int = 0x1,
    DSV_Float = 0x1,
    DSV_Left = 0x1,
    DSV_Top = 0x2,
    DSV_Right = 0x4,
    DSV_Bottom = 0x8,
    DSV_Height = 0x1,
    DSV_Width = 0x2,
    DSV_X = 0x1,
    DSV_Y = 0x2,
};

namespace DirectUI
{

	enum class ValueType : int {
    Unavailable = -2,
    Unset = -1,
    Null = 0,
    Int = 1,
    Bool = 2,
    Element = 3,
    Ellist = 4,
    String = 5,
    Point = 6,
    Size = 7,
    Rect = 8,
    Color = 9,
    Layout = 10,
		Graphic = 11,
    Sheet = 12,
    Expr = 13,
		Atom = 14,
    Cursor = 15,
		Float = 18,
		DblList = 19,
	};

    struct ScaledInt
    {
        int i;
        DynamicScaleValue dynamicScaleValue;
    };

    struct ScaledFloat
    {
        float fl;
        DynamicScaleValue dynamicScaleValue;
    };

    struct ScaledRECT : public RECT
    {
        DynamicScaleValue dynamicScaleValue;
    };

    struct ScaledSIZE : public SIZE
    {
        DynamicScaleValue dynamicScaleValue;
    };

    struct ScaledPOINT : public POINT
    {
        DynamicScaleValue dynamicScaleValue;
    };

	class UILIB_API Value
	{
    private:
        int _dType : 6;
        int _fWeakRef : 1;
        int _cRef : 25;
        const int c_RefCountBitOffset;
        const long c_SingleRefCount;
        const long c_RefCountMask;
        union $0BD36847A4748AF21FBF531C73BE30E3 {
            int _intVal;
            bool _boolVal;
            Element* _peVal;
            DynamicArray<Element*, 0>* _peListVal;
            wchar_t* _pszVal;
            POINT _ptVal;
            SIZE _sizeVal;
            RECT _rectVal;
            //struct Fill _fillVal;
            class Layout* _plVal;
            struct Graphic* _pGraphicVal;
            StyleSheet* _ppsVal;
            Expression* _pexVal;
            ATOM _atomVal;
            //struct Cursor _cursorVal;
            DynamicArray<Value*, 0>* _pvListVal;
            float _flVal;
            DynamicArray<double, 0>* _pdblListVal;
            ScaledInt _scaledIntVal;
            ScaledFloat _scaledFloatVal;
            ScaledRECT _scaledRectVal;
            ScaledSIZE _scaledSizeVal;
            ScaledPOINT _scaledPointVal;
        };
        void _ZeroRelease();
        HRESULT StrDupW(wchar_t* pszIn, wchar_t** pszOut);
        Value* CreateIconGraphicHelper(HICON hIcon, bool bFlip, bool bRTL, bool bShared);
        static HICON ReloadIcon(Graphic* pGraphic, float fScaleFactor);

    public:
        static Value* CreateInt(int dValue, DynamicScaleValue dsv);
        static Value* CreateFloat(float flValue, DynamicScaleValue dsv);
        static Value* CreateBool(bool bValue);
        static Value* CreateElementRef(Element* peValue);
        static Value* CreateElementList(DynamicArray<Element*, 0>* peListValue);
        static Value* CreateString(wchar_t* pszValue, HINSTANCE hResLoad);
        static Value* CreatePoint(int x, int y, DynamicScaleValue dsv);
        static Value* CreateSize(int cx, int cy, DynamicScaleValue dsv);
        static Value* CreateRect(int left, int top, int right, int bottom, DynamicScaleValue dsv);
        static Value* CreateColor(unsigned long cr0, unsigned long cr1, unsigned long cr2, unsigned char dType);
        static Value* CreateColor(unsigned long cr0, unsigned long cr1, unsigned char dType);
        static Value* CreateColor(unsigned long cr);
        static Value* CreateFill(const struct Fill &fill);
        static Value* CreateDFCFill(unsigned int uType, unsigned int uState);
        static Value* CreateDTBFill(wchar_t* pszClassName, int iPartId, int iStateId);
        static Value* CreateLayout(class Layout* plValue);
        static Value* CreateGraphic(wchar_t* pszICO, ScaledSIZE szDesired, HINSTANCE hResLoad, bool bFlip, bool bRTL);
        static Value* CreateGraphic(HENHMETAFILE hEnhMetaFile, HENHMETAFILE hAltEnhMetaFile);
        static Value* CreateGraphic(wchar_t* pszICO, unsigned short cxDesired, unsigned short cyDesired, HINSTANCE hResLoad, bool bFlip, bool bRTL);
        static Value* CreateGraphic(wchar_t* pszBMP, unsigned char dBlendMode, unsigned int dBlendValue, unsigned short cx, unsigned short cy, HINSTANCE hResLoad, bool bFlip, bool bRTL);
        static Value* CreateGraphic(HICON hIcon, bool bFlip, bool bRTL, bool bShared);
        static Value* CreateGraphic(HBITMAP hBitmap, unsigned char dBlendMode, unsigned int dBlendValue, bool bFlip, bool bRTL, bool bPremultiplied);
        static Value* CreateGraphic(ISharedBitmap* pBitmap, unsigned char dBlendMode, unsigned int dBlendValue);
        static Value* CreateStyleSheet(StyleSheet* ppsValue);
        static Value* CreateExpression(Expression* pexValue);
        static Value* CreateAtom(unsigned short atom);
        static Value* CreateAtom(wchar_t* pszValue);
        static Value* CreateCursor(HICON hValue);
        static Value* CreateCursor(wchar_t* pszValue);
        static Value* CreateValueList(Value* pvValue);
        static Value* CreateValueList(DynamicArray<Value*, 0>* pvListValue);
        static Value* CreateDoubleList(float* rgdbl, int cbl);
        static Value* CreateDoubleList(DynamicArray<double, 0>* pdblListValue);
        static Value* CreateStringRP(wchar_t* pszValue, HINSTANCE hResLoad);
        static Value* CreateScaledValue(float, Value* pvIn);
        static Value* CreateElementScaledValue(Element* pe, Value* pvIn);

        void AddRef();
        void Release();
        int GetRefCount();
        int GetType();
        void* GetImage(bool bGetRTL, float fScaleFactor);
        int GetInt();
        int GetScaledInt(float flScaleFactor);
        static ScaledInt* GetScaledInt();
        float GetFloat();
        bool GetBool();
        Element* GetElement();
        DynamicArray<Element*, 0>* GetElementList();
        wchar_t* GetString();
        POINT* GetPoint();
        SIZE* GetSize();
        RECT* GetRect();
        struct Fill* GetFill();
        class Layout* GetLayout();
        struct Graphic* GetGraphic();
        StyleSheet* GetStyleSheet();
        Expression* GetExpression();
        unsigned short GetAtom();
        struct Cursor* GetCursor();
        DynamicArray<Value*, 0>* GetValueList();
        DynamicArray<double, 0>* GetDoubleList();
        wchar_t* GetStringDynamicScaling();
        int GetElementScaledInt(Element*);
        void GetScaledPoint(float, POINT*);
        void GetElementScaledPoint(Element*, POINT*);
        void GetScaledSize(float, SIZE*);
        void GetElementScaledSize(Element*, SIZE*);
        void GetScaledRect(float, RECT*);
        void GetElementScaledRect(Element*, RECT*);
        float GetScaledFloat(float);
        float GetElementScaledFloat(Element*);
        bool IsDynamicScaled();
        bool IsEqual(Value*);
        wchar_t* ToString(wchar_t*, unsigned int);

        static Value* GetUnavailable();
        static Value* GetNull();
        static Value* GetUnset();
        static Value* GetElementNull();
        static Value* GetElListNull();
        static Value* GetBoolTrue();
        static Value* GetBoolFalse();
        static Value* GetStringNull();
        static Value* GetPointZero();
        static Value* GetSizeZero();
        static Value* GetRectZero();
        static Value* GetIntZero();
        static Value* GetIntMinusOne();
        static Value* GetFloatZero();
        static Value* GetFloatOne();
        static Value* GetLayoutNull();
        static Value* GetSheetNull();
        static Value* GetExprNull();
        static Value* GetAtomZero();
        static Value* GetCursorNull();
        static Value* GetColorTrans();
        static Value* GetDblListEmpty();
        static Value* GetStringRPNull();
	};

	class UILIB_API ValueProvider 
		: public PatternProvider<ValueProvider, IValueProvider, 12>
		, public IValueProvider
	{
	public:
		ValueProvider(void);
		virtual ~ValueProvider(void);
		virtual unsigned long __stdcall AddRef(void);
		virtual ProviderProxyCall GetProxyCreator(void);
		virtual long __stdcall QueryInterface(GUID const &, void * *);
		virtual unsigned long __stdcall Release(void);
		virtual long __stdcall SetValue(unsigned short const *);
		virtual long __stdcall get_IsReadOnly(int *);
		virtual long __stdcall get_Value(unsigned short * *);
	};

	class UILIB_API ValueProxy : IProxy
	{
	public:
		ValueProxy(ValueProxy const &);
		ValueProxy(void);
		ValueProxy & operator=(ValueProxy const &);
		static ValueProxy * __stdcall Create(Element *);
		static bool __stdcall IsPatternSupported(Element *);

		virtual long DoMethod(int, char *);

	protected:
		virtual void Init(Element *);

	private:
		long GetIsReadOnly(int *);
		long GetValue(unsigned short * *);
		long SetValue(unsigned short const *);
	};
}