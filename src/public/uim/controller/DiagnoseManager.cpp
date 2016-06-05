#include "StdAfx.h"
#include "DiagnoseManager.h"

#define DIAGNOSE_COMMAND_NAME _T("ShowDiagnoseTool")

CDiagnoseManager::CDiagnoseManager(void)
{
}


CDiagnoseManager::~CDiagnoseManager(void)
{
}


BOOL CDiagnoseManager::Initialize()
{
	HRESULT hr = m_pDiagnoseManager.CoCreateInstance(__uuidof(CDiagnoseManager));
	if (SUCCEEDED(hr))
	{
		m_pDiagnoseManager->Initialize();
		
		return TRUE;
	}

	return FALSE;
}


void CDiagnoseManager::Finalize()
{
	if (m_pDiagnoseManager)
	{
		m_pDiagnoseManager->Finalize();
		m_pDiagnoseManager.Release();
	}
}


BOOL CDiagnoseManager::IsDiagnoseManagerPresent()
{
	return (m_pDiagnoseManager != NULL);
}


void CDiagnoseManager::AddDownloadTest(CString strServerURL)
{
	if (m_pDiagnoseManager)
	{
		m_pDiagnoseManager->AddDownloadTest(strServerURL.AllocSysString());
	}
}


void CDiagnoseManager::AddReadFileTest(CString strPath, CString strFileName)
{
	if (m_pDiagnoseManager)
	{
		m_pDiagnoseManager->AddReadFileTest(strPath.AllocSysString(), strFileName.AllocSysString());
	}
}


void CDiagnoseManager::AddWriteFileTest(CString strPath)
{
	if (m_pDiagnoseManager)
	{
		m_pDiagnoseManager->AddWriteFileTest(strPath.AllocSysString());
	}
}


void CDiagnoseManager::AddTCPConnectionTest(CString strHost, int nPort)
{
	if (m_pDiagnoseManager)
	{
		m_pDiagnoseManager->AddTCPConnectionTest(strHost.AllocSysString(), nPort);
	}
}


void CDiagnoseManager::AddMethodTest(IBaseTest* pTest)
{
	if (pTest && m_pDiagnoseManager)
	{
		m_pDiagnoseManager->AddMethodTest(pTest);
	}
}


void CDiagnoseManager::RemoveMethodTest(IBaseTest* pTest)
{
	if (pTest && m_pDiagnoseManager)
	{
		m_pDiagnoseManager->RemoveMethodTest(pTest);
	}
}


void CDiagnoseManager::Show()
{
	if (m_pDiagnoseManager)
	{
		m_pDiagnoseManager->Show();
	}
}


CString CDiagnoseManager::GetCommandName()
{
	return DIAGNOSE_COMMAND_NAME;
}


void CDiagnoseManager::SetCookie(CString strCookieName, CString strCookieValue)
{
	if (m_pDiagnoseManager)
	{
		CComQIPtr<IDiagnoseManager2> pDiagnoseManager2 = m_pDiagnoseManager;
		if (pDiagnoseManager2 != NULL)
		{
			pDiagnoseManager2->SetCookie(strCookieName.AllocSysString(), strCookieValue.AllocSysString());
		}
	}
}


void CDiagnoseManager::SetMethod(CString strMethod)
{
	if (m_pDiagnoseManager)
	{
		CComQIPtr<IDiagnoseManager2> pDiagnoseManager2 = m_pDiagnoseManager;
		if (pDiagnoseManager2 != NULL)
		{
			pDiagnoseManager2->SetMethod(strMethod.AllocSysString());
		}
	}
}


void CDiagnoseManager::SetUrl(CString strUrl)
{
	if (m_pDiagnoseManager)
	{
		CComQIPtr<IDiagnoseManager2> pDiagnoseManager2 = m_pDiagnoseManager;
		if (pDiagnoseManager2 != NULL)
		{
			pDiagnoseManager2->SetUrl(strUrl.AllocSysString());
		}
	}
}


void CDiagnoseManager::SetUserAgent(CString strUserAgent)
{
	if (m_pDiagnoseManager)
	{
		CComQIPtr<IDiagnoseManager2> pDiagnoseManager2 = m_pDiagnoseManager;
		if (pDiagnoseManager2 != NULL)
		{
			pDiagnoseManager2->SetUserAgent(strUserAgent.AllocSysString());
		}
	}
}


void CDiagnoseManager::SetProductInfo(CString strProdName, CString strProdValue)
{
	if (m_pDiagnoseManager)
	{
		CComQIPtr<IDiagnoseManager2> pDiagnoseManager2 = m_pDiagnoseManager;
		if (pDiagnoseManager2 != NULL)
		{
			pDiagnoseManager2->SetProductInfo(strProdName.AllocSysString(), strProdValue.AllocSysString());
		}
	}
}
