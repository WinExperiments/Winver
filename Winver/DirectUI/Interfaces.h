#pragma once

namespace DirectUI
{
	struct IElementListener
	{
	public:
		//0
		virtual void OnListenerAttach(class Element* elem) = 0;
		//1
		virtual void OnListenerDetach(class Element* elem) = 0;
		//2 returns false to cancel
		virtual bool OnPropertyChanging(class Element* elem, const struct PropertyInfo *prop, int unk, class Value *before, class Value *after) = 0;
		//3
		virtual void OnListenedPropertyChanged(class Element* elem, const struct PropertyInfo *prop, int type, class Value *before, class Value *after) = 0;
		//4
		virtual void OnListenedInput(class Element* elem, struct InputEvent *event) = 0;
		//5
		virtual void OnListenedEvent(class Element* elem, struct Event *event) = 0;
	};

	struct UILIB_API IClassInfo
	{

		IClassInfo&operator=(const IClassInfo&) = delete;

		virtual void AddRef(void) = 0;
		virtual int Release(void) = 0;
		virtual HRESULT CreateInstance(Element *,ULONG *,Element * *) = 0;
		virtual const PropertyInfo * EnumPropertyInfo(UINT) = 0;
		virtual const PropertyInfo * GetByClassIndex(UINT) = 0;
		virtual unsigned int GetPICount(void) const = 0;
		virtual unsigned int GetGlobalIndex(void) const = 0;
		virtual IClassInfo * GetBaseClass(void) = 0;
		virtual UCString GetName() const = 0;
		virtual bool IsValidProperty(struct PropertyInfo const *) const = 0;
		virtual bool IsSubclassOf(IClassInfo *) const = 0;
		virtual void Destroy(void) = 0;
		virtual HINSTANCE GetModule(void) const = 0;
		virtual bool IsGlobal(void) const = 0;
		virtual void AddChild(void) = 0;
		virtual void RemoveChild(void) = 0;
		virtual int GetChildren(void) const = 0;
		virtual void AssertPIZeroRef(void) const = 0;

		//virtual ~IClassInfo();
	};

	struct UILIB_API IDataEntry
	{
	public:
		IDataEntry(IDataEntry const &);
		IDataEntry(void);
		virtual ~IDataEntry(void);

		IDataEntry & operator=(IDataEntry const &);
	};

	class DECLSPEC_NOVTABLE IProxy
	{
	public:
		virtual long DoMethod(int, char *) = 0;

	protected:
		virtual void Init(class Element *) = 0;
	};

	class UILIB_API Proxy
	{
	public:
		Proxy(Proxy const &);
		Proxy(void);
		virtual ~Proxy(void);
		Proxy & operator=(Proxy const &);

		static long __stdcall SyncCallback(struct HGADGET__ *, void *, struct EventMsg *);

	protected:
		void Invoke(unsigned int, void *);
		virtual void OnInvoke(unsigned int, void *);

	};

	class UILIB_API ProviderProxy : public IProxy
	{
	public:
		ProviderProxy(ProviderProxy const &);
		ProviderProxy & operator=(ProviderProxy const &);

	protected:
		ProviderProxy(void);
		virtual void Init(class Element *);
	};

	typedef class ProviderProxy* (__stdcall * ProviderProxyCall)(class Element *);

	class UILIB_API IProvider
	{
	public:
		IProvider(IProvider const &);
		IProvider(void);
		IProvider & operator=(IProvider const &);

		virtual ProviderProxyCall GetProxyCreator(void) = 0;
	};

	class UILIB_API RefcountBase
	{
	public:
		RefcountBase();
		RefcountBase(const RefcountBase&) = delete;
		RefcountBase&operator=(const RefcountBase&) = delete;

		virtual ~RefcountBase();

		long AddRef();
		long Release();
	};


	template <class X, class Y, int i>
	class PatternProvider
		: public RefcountBase
		, public IProvider
		//, public Y
	{
	public:
		PatternProvider();
		PatternProvider(const PatternProvider&) = delete;
		PatternProvider& operator=(const PatternProvider&) = delete;
		virtual ~PatternProvider();

		static long WINAPI Create(class ElementProvider*, IUnknown**);
		virtual void Init(class ElementProvider*);
		//IProvider
		virtual ProviderProxyCall GetProxyCreator(void);
	protected:
		long DoInvoke(int, ...);
	private:

	};


	struct UILIB_API ISBLeak
	{
	public:
		ISBLeak(ISBLeak const &);
		ISBLeak(void);
		ISBLeak & operator=(ISBLeak const &);

		virtual void T1() = 0;
		virtual void T2() = 0;
		virtual void T3() = 0;
	};

	class UILIB_API IXProviderCP
	{
	public:
		IXProviderCP(IXProviderCP const &);
		IXProviderCP(void);
		IXProviderCP & operator=(IXProviderCP const &);

		virtual long CreateDUICP(class HWNDElement *, HWND, HWND, class Element * *, class DUIXmlParser * *) = 0;
		virtual long CreateParserCP(class DUIXmlParser * *) = 0;
		virtual void DestroyCP(void) = 0;
	};

	class UILIB_API IXElementCP
	{
	public:
		IXElementCP(IXElementCP const &);
		IXElementCP(void);
		IXElementCP & operator=(IXElementCP const &);

		virtual HWND GetNotificationSinkHWND(void) = 0;
	};

	struct UILIB_API IDataEngine
	{
	public:
		IDataEngine(IDataEngine const &);
		IDataEngine(void);
		IDataEngine & operator=(IDataEngine const &);

		virtual ~IDataEngine(void);

		virtual void T1() = 0;
		virtual void T2() = 0;
	};

	struct DepRecs
	{
		int iDepPos;
		int cDepCnt;
	};

	enum LogicalOperators
	{
		PSLO_NULL = 0,
		PSLO_Equal = 1,
		PSLO_NotEqual = 2,
		PSLO_Greater = 3,
		PSLO_GreaterEqual = 4,
		PSLO_Lesser = 5,
		PSLO_LesserEqual = 6
	};

	struct Cond
	{
		PropertyInfo* ppi;
		LogicalOperators nLogOp;
		Value* pv;
	};

	struct Decl
	{
		PropertyInfo* ppi;
		Value* pv;
	};

	class UILIB_API StyleSheet
	{
	public:
		static HRESULT Create(StyleSheet** ppSheet);

		virtual void Destroy() = 0;
		virtual HRESULT AddRule(const wchar_t*, IClassInfo*, Cond*, Decl*) = 0;
		virtual void MakeImmutable() = 0;
		virtual class Value* GetSheetValue(Element*, const PropertyInfo*) = 0;
		virtual void GetSheetDependencies(Element*, const PropertyInfo*, DepRecs*, DeferCycle*, HRESULT*) = 0;
		virtual void GetSheetScope(Element*, DepRecs*, DeferCycle*, HRESULT*) = 0;
		virtual const wchar_t* GetSheetResid() = 0;
		virtual HRESULT SetSheetResid(const wchar_t*) = 0;
		virtual HRESULT SetBaseSheet(Value*, unsigned int) = 0;
	};

}

struct UILIB_API IDuiBehavior {

};
