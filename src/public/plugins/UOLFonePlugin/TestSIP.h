#pragma once

#include <util/Thread.h>
#include "../../../ctrlcenter/ctrlcenter/_ctrlcenter.h"

class CTestSIP : public CComObjectRootEx<CComSingleThreadModel>,
				 public IDispatchImpl<IBaseTest>,
				 public CThread
{
public:
	CTestSIP();
	virtual ~CTestSIP();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	BEGIN_COM_MAP(CTestSIP)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IBaseTest)
	END_COM_MAP()

protected:
	// IBaseTest
	STDMETHOD(GetTitle)(BSTR* bstrTitle);
	STDMETHOD(GetStatus)(TestStatus* pStatus);
	STDMETHOD(GetErrorDescription)(BSTR* bstrErrorDescription);
	STDMETHOD(GetResult)(BOOL* pbResult);

	STDMETHOD(StartTest)();
	STDMETHOD(CancelTest)();
	STDMETHOD(ResetTest)();
	
	STDMETHOD(RegisterObserver)(ITestObserver* pTestObserver);
	STDMETHOD(UnregisterObserver)(ITestObserver* pTestObserver);
	STDMETHOD(NotifyAll)();

private:
	void Run();

	TestStatus	m_testStatus;
	BOOL		m_bTestResult;
	CString		m_strErrorDescription;
	CAtlList<ITestObserver*> m_listObservers;
	CComAutoCriticalSection	m_csObservers;
	BOOL m_bCancel;	
};






