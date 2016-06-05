
#include "StdAfx.h"
#include "DropSource.h"


CDropSource::CDropSource() 
{
	m_lRefCount = 1;
}

CDropSource::~CDropSource()
{
}

// IUnknown methods
HRESULT __stdcall CDropSource::QueryInterface(REFIID iid, void **ppvObject)
{
    if(iid == IID_IDropSource || iid == IID_IUnknown)
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

ULONG __stdcall CDropSource::AddRef(void)
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDropSource::Release(void)
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

// IDropSource methods
HRESULT __stdcall CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	if(fEscapePressed == TRUE)
	{
		return DRAGDROP_S_CANCEL;	
	}

	if((grfKeyState & MK_LBUTTON) == 0)
	{
		return DRAGDROP_S_DROP;
	}

	return S_OK;
}

HRESULT __stdcall CDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}