#pragma once

template <class TBase>
class CDropTarget : 
	public TBase, 
	public IDropTarget
{
public:
	CDropTarget();
	~CDropTarget();

	// IUnknow methods
	HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
	ULONG __stdcall AddRef (void);
	ULONG __stdcall Release (void);

	// IDropTarget methods
	HRESULT __stdcall DragEnter (IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	HRESULT __stdcall DragOver (DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);	
	HRESULT __stdcall DragLeave (void);	
	HRESULT __stdcall Drop (IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

protected:
	virtual BOOL IsValidObject(IDataObject *pDataObject) = 0;
	virtual DWORD DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed) = 0;
	virtual DWORD DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed) = 0;
	virtual void DoDragLeave() = 0;
	virtual DWORD DoDrop(IDataObject *pDataObject, POINTL pt) = 0;

protected:
	BEGIN_MSG_MAP(CDropTarget)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy(void);

private:
	LONG				m_lRefCount;
	BOOL				m_fAllowDrop;
	IDropTargetHelper*	m_piDropHelper;
	bool				m_bUseDnDHelper;
};

template <class TBase>
CDropTarget<TBase>::CDropTarget(void) : 
	m_lRefCount(1),
	m_fAllowDrop(FALSE),
	m_bUseDnDHelper(false),
	m_piDropHelper(NULL)
{
	// Create drag and drop helper object.
	if (SUCCEEDED(CoCreateInstance (CLSID_DragDropHelper, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper, 
		(void**) &m_piDropHelper)))
	{
		m_bUseDnDHelper = true;
	}
}

template <class TBase>
CDropTarget<TBase>::~CDropTarget(void) 
{
	if (m_piDropHelper != NULL)
	{
        m_piDropHelper->Release();
	}
}

template <class TBase>
HRESULT __stdcall CDropTarget<TBase>::QueryInterface (REFIID iid, void ** ppvObject)
{
	if(iid == IID_IDropTarget || iid == IID_IUnknown)
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

//IUnknow methods
template <class TBase>
ULONG __stdcall CDropTarget<TBase>::AddRef (void)
{
	return InterlockedIncrement(&m_lRefCount);
}

template <class TBase>
ULONG __stdcall CDropTarget<TBase>::Release (void)
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

// IDropTarget methods
template <class TBase>
HRESULT __stdcall CDropTarget<TBase>::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	m_fAllowDrop = IsValidObject(pDataObject);

	if(m_fAllowDrop)
	{
		*pdwEffect = DoDragEnter(grfKeyState, pt, *pdwEffect);
		SetFocus();
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	// Call the drag and drop helper
	if (m_bUseDnDHelper)
	{
		POINT point;
		point.x = pt.x;
		point.y = pt.y;
		if (m_piDropHelper != NULL)
		{
			m_piDropHelper->DragEnter(m_hWnd, pDataObject, &point, *pdwEffect);
		}
	}

	return S_OK;
}

template <class TBase>
HRESULT __stdcall CDropTarget<TBase>::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if(m_fAllowDrop)
	{
		*pdwEffect = DoDragOver(grfKeyState, pt, *pdwEffect);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	// Call the DnD helper
	if (m_bUseDnDHelper)
	{
		POINT point;
		point.x = pt.x;
		point.y = pt.y;
		if (m_piDropHelper != NULL)
		{
			m_piDropHelper->DragOver(&point, *pdwEffect);
		}
	}

	return S_OK;
}

template <class TBase>
HRESULT __stdcall CDropTarget<TBase>::DragLeave (void)
{
	// Call the DnD helper
	if (m_bUseDnDHelper)
    {
		if (m_piDropHelper != NULL)
		{
			m_piDropHelper->DragLeave();
		}
    }

	DoDragLeave();

	return S_OK;
}

template <class TBase>
HRESULT __stdcall CDropTarget<TBase>::Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	*pdwEffect = DROPEFFECT_NONE;

	if (m_fAllowDrop)
	{
		*pdwEffect = DoDrop(pDataObject, pt);
	}

	// Call the DnD helper
	if (m_bUseDnDHelper)
	{
		POINT point;
		point.x = pt.x;
		point.y = pt.y;		
		if (m_piDropHelper != NULL)
		{
			m_piDropHelper->Drop(pDataObject, &point, *pdwEffect);
		}
	}

	return S_OK;
}

//Messages
template <class TBase>
LRESULT CDropTarget<TBase>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	RegisterDragDrop(m_hWnd, this);
	SetMsgHandled(FALSE);

	return 0L;
}

template <class TBase>
void CDropTarget<TBase>::OnDestroy(void)
{
	RevokeDragDrop(m_hWnd);
	SetMsgHandled(FALSE);
}