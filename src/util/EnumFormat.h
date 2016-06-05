#pragma once

static void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source)
{
	*dest = *source;
	
	if(source->ptd)
	{
		dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

		*(dest->ptd) = *(source->ptd);
	}
}

class CEnumFormatEtc : public IEnumFORMATETC
{
public:

	CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats);
	~CEnumFormatEtc();

    // IUnknown methods
	HRESULT __stdcall  QueryInterface (REFIID iid, void ** ppvObject);
	ULONG	__stdcall  AddRef (void);
	ULONG	__stdcall  Release (void);

	// IEnumFORMATETC methods
	HRESULT __stdcall  Next  (ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched);
	HRESULT __stdcall  Skip  (ULONG celt); 
	HRESULT __stdcall  Reset (void);
	HRESULT __stdcall  Clone (IEnumFORMATETC ** ppEnumFormatEtc);

private:

	LONG		m_lRefCount;		
	ULONG		m_nIndex;			
	ULONG		m_nNumFormats;		
	FORMATETC * m_pFormatEtc;		
};
