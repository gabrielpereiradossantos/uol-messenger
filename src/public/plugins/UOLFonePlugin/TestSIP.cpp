#include "StdAfx.h"
#include "TestSIP.h"
#include "Core/UOLFoneController.h"
#include "Core/UOLFoneFailureInfo.h"


CTestSIP::CTestSIP(void) :
	m_bCancel(FALSE),
	m_testStatus(TEST_NOT_STARTED),
	m_bTestResult(FALSE)
{
}


CTestSIP::~CTestSIP(void)
{
	m_listObservers.RemoveAll();
}


STDMETHODIMP CTestSIP::GetTitle(BSTR* bstrTitle)
{
	CString strTitle = "Teste de conectividade SIP";
	*bstrTitle = strTitle.AllocSysString();
	
	return S_OK;
}


STDMETHODIMP CTestSIP::GetStatus(TestStatus* pStatus)
{
	*pStatus = m_testStatus;
	return S_OK;
}


STDMETHODIMP CTestSIP::GetErrorDescription(BSTR* bstrErrorDescription)
{
	*bstrErrorDescription = m_strErrorDescription.AllocSysString();
	return S_OK;
}


STDMETHODIMP CTestSIP::GetResult(BOOL* pbResult)
{
	*pbResult = m_bTestResult;
	return S_OK;
}


STDMETHODIMP CTestSIP::StartTest()
{
	m_bCancel = FALSE;
	m_testStatus = TEST_STARTED;

	// Start thread
	Start();

	return S_OK;
}


void CTestSIP::Run()
{
	m_bTestResult = CUOLFoneController::GetInstance()->TestNetworkConnection();
	if (!m_bTestResult)
	{
		CUOLFoneFailureInfoPtr pLastFailureInfo;
		pLastFailureInfo = CUOLFoneController::GetInstance()->GetLastFailureInfo();
		if (pLastFailureInfo)
		{
			m_strErrorDescription = pLastFailureInfo->GetFailureInfoText();
		}
	}

	m_testStatus = TEST_FINISHED;

	NotifyAll();
}


STDMETHODIMP CTestSIP::CancelTest()
{
	m_bCancel = TRUE;
	m_testStatus = TEST_CANCELED;

	return S_OK;
}


STDMETHODIMP CTestSIP::ResetTest()
{
	m_bCancel = FALSE;
	m_testStatus = TEST_NOT_STARTED;
	m_bTestResult = FALSE;

	return S_OK;
}


STDMETHODIMP CTestSIP::RegisterObserver(ITestObserver* pTestObserver)
{
	if (pTestObserver != NULL)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
		
		POSITION pos = m_listObservers.Find(pTestObserver);

		if (!pos)
		{
			m_listObservers.AddTail(pTestObserver);
		}
	}

	return S_OK;
}


STDMETHODIMP CTestSIP::UnregisterObserver(ITestObserver* pTestObserver)
{
	if (pTestObserver != NULL)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		POSITION pos = m_listObservers.Find(pTestObserver);

		if (pos != NULL)
		{
			m_listObservers.RemoveAt(pos);
		}
	}

	return S_OK;
}


STDMETHODIMP CTestSIP::NotifyAll()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
	
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		ITestObserver* pTestObserver = m_listObservers.GetNext(pos);

		pTestObserver->OnTestUpdate();
	}

	return S_OK;
}
