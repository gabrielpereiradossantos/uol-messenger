
#include "StdAfx.h"
#include "DataObject.h"


HGLOBAL DupMem(HGLOBAL hMem)
{
	DWORD   len    = GlobalSize(hMem);
	PVOID   source = GlobalLock(hMem);
	
	PVOID   dest   = GlobalAlloc(GMEM_FIXED, len);

	memcpy(dest, source, len);

	GlobalUnlock(hMem);

	return dest;
}

CDataObject::CDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count) 
{
	m_lRefCount  = 1;
	m_nNumFormats = count;
	
	m_pFormatEtc  = new FORMATETC[count];
	m_pStgMedium  = new STGMEDIUM[count];

	for(int i = 0; i < count; i++)
	{
		m_pFormatEtc[i] = fmtetc[i];
		m_pStgMedium[i] = stgmed[i];
	}
}

CDataObject::~CDataObject()
{
	if(m_pFormatEtc) delete[] m_pFormatEtc;
	if(m_pStgMedium) delete[] m_pStgMedium;
}

// IUnknown methods
HRESULT __stdcall CDataObject::QueryInterface(REFIID iid, void **ppvObject)
{
    if(iid == IID_IDataObject || iid == IID_IUnknown)
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

ULONG __stdcall CDataObject::AddRef(void)
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDataObject::Release(void)
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

// IDataObject methods
HRESULT __stdcall CDataObject::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
	int idx;

	if((idx = LookupFormatEtc(pFormatEtc)) == -1)
	{
		return DV_E_FORMATETC;
	}

	pMedium->tymed			 = m_pFormatEtc[idx].tymed;
	pMedium->pUnkForRelease  = 0;
	
	switch(m_pFormatEtc[idx].tymed)
	{
	case TYMED_HGLOBAL:

		pMedium->hGlobal = DupMem(m_pStgMedium[idx].hGlobal);
		break;

	default:
		return DV_E_FORMATETC;
	}

	return S_OK;
}

HRESULT __stdcall CDataObject::GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
	return DATA_E_FORMATETC;
}

HRESULT __stdcall CDataObject::QueryGetData(FORMATETC *pFormatEtc)
{
	return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}

HRESULT __stdcall CDataObject::GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
}

HRESULT __stdcall CDataObject::SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{
	if(dwDirection == DATADIR_GET)
	{
		*ppEnumFormatEtc = new CEnumFormatEtc(m_pFormatEtc, m_nNumFormats);

		return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT __stdcall CDataObject::DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT __stdcall CDataObject::DUnadvise (DWORD dwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT __stdcall CDataObject::EnumDAdvise (IEnumSTATDATA **ppEnumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

// Private methods
int CDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
	for(int i = 0; i < m_nNumFormats; i++)
	{
		if((pFormatEtc->tymed    &  m_pFormatEtc[i].tymed)   &&
			pFormatEtc->cfFormat == m_pFormatEtc[i].cfFormat && 
			pFormatEtc->dwAspect == m_pFormatEtc[i].dwAspect)
		{
			return i;
		}
	}

	return -1;
}