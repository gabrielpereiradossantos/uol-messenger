#include "stdafx.h"
#include "comdll.h"


CComDll::CComDll()
{
	m_hDll = NULL;
}


CComDll::~CComDll()
{
	UnloadDll();
}


BOOL CComDll::LoadDll(const CString& strDllPath)
{
	m_hDll = ::LoadLibrary(strDllPath);

	return (m_hDll != NULL);
}


BOOL CComDll::UnloadDll()
{
	BOOL bRet = ::FreeLibrary(m_hDll);
	
	if (bRet)
	{	
		m_hDll = NULL;
	}

	return bRet;
}


HRESULT CComDll::CoCreateInstance(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	typedef HRESULT (__stdcall *fnDllGetClassObject)(REFCLSID, REFIID, LPVOID*);

	fnDllGetClassObject pfnDllGetClassObject = (fnDllGetClassObject)::GetProcAddress(m_hDll, "DllGetClassObject");

	if (NULL == pfnDllGetClassObject)
	{
		return E_FAIL;
	}

	CComPtr<IClassFactory> p;

	HRESULT hr = pfnDllGetClassObject(rclsid, __uuidof(IClassFactory), (void**)&p);	

	if (FAILED(hr))
	{
		return hr;
	}

	return p->CreateInstance(NULL, riid, ppv);	
}