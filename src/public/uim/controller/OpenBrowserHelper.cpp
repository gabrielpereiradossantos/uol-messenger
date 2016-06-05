/* UOL Messenger
* Copyright (c) 2005 Universo Online S/A
*
* Direitos Autorais Reservados
* All rights reserved
*
* Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
* sob os termos da Licença Pública Geral GNU conforme publicada pela Free
* Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
* qualquer versão posterior.
* Este programa é distribuído na expectativa de que seja útil, porém,
* SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
* OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
* do GNU para mais detalhes. 
* Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
* com este programa; se não, escreva para a Free Software Foundation, Inc.,
* no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
* Universo Online S/A - A/C: UOL Messenger 5o. Andar
* Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
* São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "OpenBrowserHelper.h"

#include <HighPerformanceCounter.h>


COpenBrowserHelper::COpenBrowserHelper() :  
m_bDone(FALSE)
{
}


COpenBrowserHelper::~COpenBrowserHelper()
{
}


void COpenBrowserHelper::Navigate(const CString& strURL)
{
	if (!strURL.IsEmpty())
	{
		CComPtr<IWebBrowser2> pWebBrowser;

		HRESULT hr = pWebBrowser.CoCreateInstance(CLSID_InternetExplorer, NULL);

		if (pWebBrowser)
		{
			CComVariant vURL = strURL;
			CComVariant vFlags = navOpenInNewWindow;
			CComVariant vTarget = _T("_BLANK");
			CComVariant vDummy;

			hr = pWebBrowser->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);
			ATLASSERT(SUCCEEDED(hr));

			m_pWebBrowser.Release();
		}
	}
}

void COpenBrowserHelper::Navigate(const CString& strURL, const CString& strCookie, const CString& strCookieURL)
{
	if ((!strCookieURL.IsEmpty()) && 
		(!strCookie.IsEmpty()) &&
		(!strURL.IsEmpty()))
	{
		if (IsIE7OrNewer())
		{
			NavigateIE7(strURL, strCookie, strCookieURL);
		}
		else
		{
			NavigateIE(strURL, strCookie, strCookieURL);
		}
	}
}

void COpenBrowserHelper::NavigateIE(const CString& strURL, const CString& strCookie, const CString& strCookieURL)
{
	HRESULT hr = m_pWebBrowser.CoCreateInstance(CLSID_InternetExplorer, NULL);

	if (m_pWebBrowser)
	{
		CComVariant vURL;
		CComVariant vFlags = navOpenInNewWindow;
		CComVariant vTarget = _T("_BLANK");
		CComVariant vDummy;

		m_bDone = FALSE;

		hr = ReceiveEvents(TRUE, m_pWebBrowser);
		ATLASSERT(SUCCEEDED(hr));

		// Navigate to cookie URL
		vURL = strCookieURL;
		hr = m_pWebBrowser->Navigate2(&vURL, &vDummy, &vDummy, &vDummy, &vDummy);

		ATLASSERT(SUCCEEDED(hr));

		ATLTRACE(_T("Waiting document loading...\n"));

		// Wait HTML document load completion
		m_pThread = new CThread(this);
		m_pThread->Start();
		m_pThread->MessageLoopWait();

		hr = ReceiveEvents(FALSE, m_pWebBrowser);
		ATLASSERT(SUCCEEDED(hr));

		ATLTRACE(_T("Getting document...\n"));

		// Put cookie into document and open URL using new window
		CComPtr<IDispatch> pDispatch;

		hr = m_pWebBrowser->get_Document(&pDispatch);

		if (SUCCEEDED(hr))
		{
			CComPtr<IHTMLDocument2> pDocument;

			pDocument = pDispatch;

			if (pDocument != NULL)
			{
				ATLTRACE(_T("Setting cookie...\n"));

				hr = pDocument->put_cookie(CComBSTR(strCookie));

				ATLASSERT(SUCCEEDED(hr));
			}
		}

		ATLTRACE(_T("Opening URL...\n"));

		vURL = strURL;
		hr = m_pWebBrowser->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);

		ATLASSERT(SUCCEEDED(hr));

		m_pWebBrowser.Release();
	}
}


void COpenBrowserHelper::Run()
{
	HighPerformanceCounter counter;

	if (counter.Start())
	{
		const LONG DOCUMENT_COMPLETE_WAIT_TIMEOUT = 30;
		LONG nElapsedSeconds;

		while (!m_bDone)
		{
			nElapsedSeconds = static_cast<LONG>(counter.Stop());

			if (nElapsedSeconds > DOCUMENT_COMPLETE_WAIT_TIMEOUT)
			{
				ATLTRACE(_T("%s - Timeout!!!\n"), __FUNCTION__);
				break;
			}

			::Sleep(500);
		}
	}
}


STDMETHODIMP_(void) COpenBrowserHelper::DocumentComplete(IDispatch* pDisp, VARIANT* URL)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_bDone = TRUE;
}

void COpenBrowserHelper::NavigateIE7(const CString& strURL, const CString& strCookie, const CString& strCookieURL)
{  
	DWORD dwIEProc = 0;
	CComVariant vURL;
	CComVariant vFlags = vtMissing;

	CComVariant vTarget = _T("_parent");
	CComVariant vDummy;

	// Load IEFrame.dll
	HMODULE hIEFrameModule = LoadLibrary(_T("ieframe.dll"));   
	if (!hIEFrameModule)  
		return;  
	PFN_IELaunchURL pfnIELaunchURL = (PFN_IELaunchURL)GetProcAddress(hIEFrameModule, _T("IELaunchURL"));   

	if (!pfnIELaunchURL)  
	{  
		FreeLibrary(hIEFrameModule);  
		return;  
	}  

	// Load oleacc.dll
	HMODULE hOleAccModule = LoadLibrary(_T("oleacc.dll"));   
	if (!hOleAccModule)  
		return;  
	PFN_OBJECTFROMLRESULT pfnOleAccModule = (PFN_OBJECTFROMLRESULT)GetProcAddress(hOleAccModule, _T("ObjectFromLresult"));   

	if (!pfnOleAccModule)  
	{  
		FreeLibrary(hOleAccModule);  
		return;  
	}  
	
	// Launch URL
	PROCESS_INFORMATION pProcInfo;  
	HRESULT hr = (pfnIELaunchURL)(CT2CW((LPCTSTR) strCookieURL), &pProcInfo, NULL);  

	if (FAILED(hr))  
	{  
		FreeLibrary(hIEFrameModule);  
		return;  
	}  

	// Wait for IE server response
	HWND hWndIEFrame = NULL;  
	HWND hWnd = NULL;  
	int nWait = 100;  
	while ((hWnd == NULL) && (nWait > 0))  
	{  
		nWait--;  
		Sleep(1000);  
		WaitForInputIdle(pProcInfo.hProcess, 30000);  
		if (hWndIEFrame == NULL)  
			EnumThreadWindows(pProcInfo.dwThreadId, &EnumWinProc, (LPARAM)&hWndIEFrame);  

		if (hWndIEFrame)  
			EnumChildWindows(hWndIEFrame, &EnumChildWinProc, (LPARAM)&hWnd);  
	}  

	WaitForInputIdle(pProcInfo.hProcess, 30000);  
	dwIEProc = pProcInfo.dwProcessId;
	CloseHandle(pProcInfo.hProcess);  
	CloseHandle(pProcInfo.hThread);   

	if (hWnd == NULL)  
	{  
		FreeLibrary(hIEFrameModule);  
		return;  
	}  

	CoInitialize(NULL);
	
	// Get m_pWebBrowser
	CComPtr<IHTMLDocument2> spDoc;  
    LRESULT lRes;  
  
    UINT nMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));  
    ::SendMessageTimeout(hWnd, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 10000, (DWORD*)&lRes);  
  
	// Call OleAccModule
	hr = (pfnOleAccModule)(lRes, __uuidof(IHTMLDocument2), 0, (void**)&spDoc);
    if (FAILED(hr))
    {  
        FreeLibrary(hOleAccModule);  
        return;  
    }  
	
	CComQIPtr<IServiceProvider> spsp;  
    spsp = spDoc;  
    spsp->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&m_pWebBrowser);  
	
	// Process url page
	if (m_pWebBrowser)
	{
		// Put cookie into document and open URL using new window
		CComPtr<IDispatch> pDispatch;

		hr = m_pWebBrowser->get_Document(&pDispatch);
		if (SUCCEEDED(hr))
		{
			CComPtr<IHTMLDocument2> pDocument;
			pDocument = pDispatch;
			if (pDocument != NULL)
			{
				ATLTRACE(_T("Setting cookie...\n"));
				hr = pDocument->put_cookie(CComBSTR(strCookie));
				ATLASSERT(SUCCEEDED(hr));
			}
		}

		ATLTRACE(_T("Opening URL...\n"));

		vURL = strURL;
		hr = m_pWebBrowser->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);
		ATLASSERT(SUCCEEDED(hr));

		m_pWebBrowser.Release();
	}
	CoUninitialize();
	FreeLibrary(hIEFrameModule);
}  

BOOL CALLBACK EnumChildWinProc(HWND hWnd, LPARAM lParam)  
{  
	TCHAR  szClassName[256];  
	GetClassName(hWnd, (LPTSTR)&szClassName, 256);  

	if (_tcscmp(szClassName, _T("Internet Explorer_Server")) == 0)  
	{  
		// Internet Explorer_Server
		HWND hParent = GetParent(hWnd);  

		if (hParent)  
		{  
			hParent = GetParent(hParent);  
			if (hParent)  
			{  
				GetClassName(hParent, (LPTSTR)&szClassName, 256);  
				if (_tcscmp(szClassName, _T("TabWindowClass")) == 0)  
				{  
					*(HWND*)lParam = hWnd;  
					return FALSE;  
				}  
			}  
		}  
	}  
	return TRUE;  
}  

BOOL CALLBACK EnumWinProc(HWND hWnd, LPARAM lParam)  
{  
	TCHAR  szClassName[256];  
	GetClassName(hWnd, (LPTSTR)&szClassName, 256);  

	if (_tcscmp(szClassName, _T("IEFrame")) == 0)  
	{  
		*(HWND*)lParam = hWnd;  
		return FALSE;  
	}  
	return TRUE;  
}

CString COpenBrowserHelper::GetInternetExplorerVersion()
{
	CString strVersion="none";
	CRegKey regInfo;
	
	BOOL boolKeyOK = ERROR_SUCCESS == regInfo.Open(
			HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Internet Explorer", KEY_READ);
	
	if (boolKeyOK)
	{
		ULONG ulLength=1024;
		TCHAR* pchData = strVersion.GetBufferSetLength(ulLength);
		
		regInfo.QueryStringValue("Version", pchData, &ulLength);
		
		strVersion.ReleaseBuffer();
	}
	
	return 	strVersion;
}

BOOL COpenBrowserHelper::IsIE7OrNewer()
{
	CString strVersion="0";
	CRegKey regInfo;
	
	BOOL boolKeyOK = ERROR_SUCCESS == regInfo.Open(
			HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Internet Explorer", KEY_READ);
	
	if (boolKeyOK)
	{
		ULONG ulLength=1024;
		TCHAR* pchData = strVersion.GetBufferSetLength(ulLength);
		
		regInfo.QueryStringValue("Version", pchData, &ulLength);
		
		strVersion.ReleaseBuffer();
	}
	
	return atol((LPCTSTR)strVersion) >= 7;
}