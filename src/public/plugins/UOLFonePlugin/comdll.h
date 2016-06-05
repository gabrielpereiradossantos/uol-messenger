#pragma once

class CComDll
{
public:
	CComDll();
	virtual ~CComDll();

	BOOL LoadDll(const CString& strDllPath);
	BOOL UnloadDll();
	HRESULT CoCreateInstance(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

private:
	HINSTANCE	m_hDll;
};