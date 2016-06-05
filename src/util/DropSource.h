#pragma once

class CDropSource : public IDropSource
{
public:
	CDropSource();
	~CDropSource();

	// IUnknown methods
	HRESULT __stdcall QueryInterface	(REFIID iid, void ** ppvObject);
	ULONG   __stdcall AddRef			(void);
	ULONG   __stdcall Release			(void);

	// IDropSource methods
	HRESULT __stdcall QueryContinueDrag	(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback		(DWORD dwEffect);	

private:
	LONG	   m_lRefCount;
};
