
#include "StdAfx.h"
#include "EnumFormat.h"

CEnumFormatEtc::CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats)
{
	m_lRefCount   = 1;
	m_nIndex      = 0;
	m_nNumFormats = nNumFormats;
	m_pFormatEtc  = new FORMATETC[nNumFormats];
	
	for(int i = 0; i < nNumFormats; i++)
	{	
		DeepCopyFormatEtc(&m_pFormatEtc[i], &pFormatEtc[i]);
	}
}

CEnumFormatEtc::~CEnumFormatEtc()
{
	if(m_pFormatEtc)
	{
		for(ULONG i = 0; i < m_nNumFormats; i++)
		{
			if(m_pFormatEtc[i].ptd)
				CoTaskMemFree(m_pFormatEtc[i].ptd);
		}

		delete[] m_pFormatEtc;
	}
}

// IUnknown methods
HRESULT __stdcall CEnumFormatEtc::QueryInterface(REFIID iid, void **ppvObject)
{
    if(iid == IID_IEnumFORMATETC || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
}

ULONG __stdcall CEnumFormatEtc::AddRef(void)
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CEnumFormatEtc::Release(void)
{
	LONG count = InterlockedDecrement(&m_lRefCount);
		
	if(count == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}

// IEnumFORMATETC methods
HRESULT __stdcall CEnumFormatEtc::Next(ULONG celt, FORMATETC *pFormatEtc, ULONG * pceltFetched)
{
	ULONG copied  = 0;

	if(celt == 0 || pFormatEtc == 0)
		return E_INVALIDARG;

	while(m_nIndex < m_nNumFormats && copied < celt)
	{
		DeepCopyFormatEtc(&pFormatEtc[copied], &m_pFormatEtc[m_nIndex]);
		copied++;
		m_nIndex++;
	}

	if(pceltFetched != 0) 
		*pceltFetched = copied;

	return (copied == celt) ? S_OK : S_FALSE;
}

HRESULT __stdcall CEnumFormatEtc::Skip(ULONG celt)
{
	m_nIndex += celt;
	return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}

HRESULT __stdcall CEnumFormatEtc::Reset(void)
{
	m_nIndex = 0;
	return S_OK;
}

HRESULT __stdcall CEnumFormatEtc::Clone(IEnumFORMATETC ** ppEnumFormatEtc)
{
	*ppEnumFormatEtc = new CEnumFormatEtc(m_pFormatEtc, m_nNumFormats);

	if(*ppEnumFormatEtc)
	{
		((CEnumFormatEtc *) *ppEnumFormatEtc)->m_nIndex = m_nIndex;
	}

	return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
}