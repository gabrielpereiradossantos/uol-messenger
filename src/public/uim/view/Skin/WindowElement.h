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
 
#pragma once
#include "element.h"
#include "BackgroundElement.h"
#include <interfaces/IUOLMessengerCommand.h>

class __declspec(novtable) IWindowElement 
{
public:	
	virtual ~IWindowElement() {}
    
	virtual CWindow	CreateWnd(HWND hWndParent) = 0; 
	virtual void DrawBackground(CDCHandle& dc, const CRect& rect) = 0;	
	virtual void GetMinSize(CSize& size) = 0;
	virtual void DoPaint(CDCHandle& dcHandle) = 0;
	virtual BOOL HitTest(const CPoint& pt, int& nTest) const = 0;
	virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
	virtual CWindow GetWindow() const = 0;		
	virtual IUOLMessengerCommandPtr GetCommand(const CString& strCommand) = 0;
	virtual CRgnHandle GetClippingRgn(const CRect& rectWindow) const = 0;
	virtual void SetBackground(CBackgroundElementPtr pBackGround) = 0;
	virtual void SetBackground(CBackgroundElementPtr pActiveBackground, CBackgroundElementPtr pInactiveBackground) = 0;	
};

MAKEAUTOPTR(IWindowElement);

template <typename T, typename TBase>
class __declspec(novtable) CWindowElement :	
	public TBase,
    public CElement, 
	public IWindowElement
{
public:
	CWindowElement(void);
	virtual ~CWindowElement(void);	

	
protected:
	BEGIN_MSG_MAP(CWindowElement)	
		MSG_WM_ERASEBKGND(OnEraseBackground);
		MSG_WM_PAINT(OnPaint)
		MSG_WM_SIZE(OnSize)
		MSG_WM_ACTIVATE(OnActivate)
		/*MSG_WM_PRINT(OnPrint)
		MSG_WM_PRINTCLIENT(OnPrintClient)*/
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnEraseBackground(HDC hDc);
	void OnPaint(HDC hDc);
	void OnSize(UINT, const CSize& size);
	void OnActivate(UINT, BOOL, HWND);
	//void OnPrint(HDC, UINT);
	//void OnPrintClient(HDC, UINT);

	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);	
	virtual void DrawBackground(CDCHandle& dc, const CRect& rect);		
	virtual CWindow	CreateWnd(HWND hWndParent);
	virtual void GetMinSize(CSize& size);
	virtual void DoPaint(CDCHandle& dc);
	virtual BOOL HitTest(const CPoint& pt, int& nTest) const;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CWindow GetWindow() const;	
	virtual IUOLMessengerCommandPtr GetCommand(const CString& strCommand);
	virtual BOOL OnActivate(BOOL);
	CRgnHandle GetClippingRgn(const CRect& rectWindow) const;	
	virtual void SetBackground(CBackgroundElementPtr pBackGround);
	virtual void SetBackground(CBackgroundElementPtr pActiveBackground, CBackgroundElementPtr pInactiveBackground);

private:

protected:	
	enum DockMode
	{
		dockNo = 0,
		dockLeft = 1, 
		dockTop = 2, 
		dockRight = 4,
		dockBottom = 8
	};

	CBackgroundElementPtr	m_pActiveBackground;	
	CBackgroundElementPtr	m_pInactiveBackground;	
	
	CBackgroundElement*		m_pCurrentBackground;	

	int						m_nOpacity;
};







template <typename T, typename TBase>
CWindowElement<T, TBase>::CWindowElement(void) :
	m_pCurrentBackground(NULL), 
	m_nOpacity(100)
{
}

template <typename T, typename TBase>
CWindowElement<T, TBase>::~CWindowElement(void)
{
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());

	int y = 0;	

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		IElement* pElement = m_listChilds.GetNext(pos).get();

		if (IWindowElement* pWindowElement = dynamic_cast<IWindowElement*>(pElement))
		{			
			CSize sizeElement;
			pWindowElement->GetMinSize(sizeElement);

			CWindow* pWindow = dynamic_cast<CWindow*>(pWindowElement);

			if (NULL == pos)
			{
				pWindow->SetWindowPos(NULL, 0, y, size.cx, size.cy - y,  SWP_NOZORDER); 						
			}
			else
			{
				pWindow->SetWindowPos(NULL, 0, y, size.cx, sizeElement.cy,  SWP_NOZORDER); 						
				y += sizeElement.cy;
			}			
		}
	}

	CRgnHandle rgn = GetClippingRgn(rect);

	if (rgn)
	{
		SetWindowRgn(rgn, TRUE);	
	}
}

template <typename T, typename TBase>
LRESULT CWindowElement<T, TBase>::OnEraseBackground(HDC hDc)
{
	return 1;
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::OnPaint(HDC hDc)
{
	CRect rect;
	GetClientRect(&rect);
	CPaintDC dc(m_hWnd);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);

	if (m_nOpacity == 100)
	{
		CMemDC memDC2(dc, &rect);
		CDCHandle dcHandle(memDC2);

		dc.SaveDC();
		dc.SelectClipRgn(rgn);	

		static_cast<T*>(this)->DrawBackground(dcHandle, rect);	
		static_cast<T*>(this)->DoPaint(dcHandle);

		memDC2.Update();

		dc.RestoreDC(-1);
	}
	else
	{
		CMemDC memDC(dc, &rect, m_nOpacity);
		CMemDC memDC2(memDC, &rect);

		CDCHandle dcHandle(memDC2);

		dc.SaveDC();
		dc.SelectClipRgn(rgn);	

		static_cast<T*>(this)->DrawBackground(dcHandle, rect);	

		memDC2.Update();

		static_cast<T*>(this)->DoPaint(dcHandle);

		dc.RestoreDC(-1);
	}	
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::OnSize(UINT nType, const CSize& size)
{
	if (SIZE_MINIMIZED != nType)
	{
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
	}

	static_cast<T*>(this)->SetMsgHandled(FALSE);
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::OnActivate(UINT nActivate, BOOL, HWND)
{
	BOOL bOK = FALSE;

	switch (nActivate)
	{
	case WA_ACTIVE:
	case WA_CLICKACTIVE:
		bOK = static_cast<T*>(this)->OnActivate(TRUE);		
		break;

	case WA_INACTIVE:
		bOK = static_cast<T*>(this)->OnActivate(FALSE);
		break;
	}

	SetMsgHandled(bOK);	
}

//template <typename T, typename TBase>
//void CWindowElement<T, TBase>::OnPrint(HDC hDc, UINT)
//{
//	CDCHandle dc(hDc);
//
//	DoPaint(dc);
//}
//
//template <typename T, typename TBase>
//void OnPrintClient(HDC hDc, UINT)
//{
//	CDCHandle dc(hDc);
//
//	DoPaint(dc);
//}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	CBackgroundElementPtr pInactiveBackground;
	CBackgroundElementPtr pActiveBackground;

	try
	{
		CString strBackground;
		GetParameterValue(strBackground, pMapParams, "background");

		pActiveBackground = ap_dynamic_cast<CBackgroundElementPtr>(pElementTable->GetElement(strBackground));
		ATLASSERT(pActiveBackground);		

		try
		{
			CString strBackground;
			GetParameterValue(strBackground, pMapParams, "inactive_background");

			pInactiveBackground= ap_dynamic_cast<CBackgroundElementPtr>(pElementTable->GetElement(strBackground));
			ATLASSERT(pInactiveBackground);
		}
		catch(const CSkinException&)
		{
			SetBackground(pActiveBackground);
		}

		SetBackground(pActiveBackground, pInactiveBackground);
	}
	catch(const CSkinException&)
	{
	}


	try
	{	
		GetParameterValue(m_nOpacity, pMapParams, "opacity");	

		// Valid range: 0 - 100
		// 0	- transparent
		// 100  - opaque
		m_nOpacity = max(0, min(100, m_nOpacity)); 
	}
	catch(const CSkinException&)
	{
	}
	
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::DrawBackground(CDCHandle& dc, const CRect& rect)
{
	if (m_pCurrentBackground)
	{	
		m_pCurrentBackground->Draw(dc, rect);
	}
}


template <typename T, typename TBase>
CWindow	CWindowElement<T, TBase>::CreateWnd(HWND hWndParent)
{
	return TBase::Create(hWndParent);
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::GetMinSize(CSize& size)
{
	size = CSize(0, 0);
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::DoPaint(CDCHandle& dc)
{	
}

template <typename T, typename TBase>
BOOL CWindowElement<T, TBase>::HitTest(const CPoint& pt, int& nTest) const
{
	nTest = HTTRANSPARENT;

	return TRUE;
}

template <typename T, typename TBase>
BOOL CWindowElement<T, TBase>::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

template <typename T, typename TBase>
CWindow CWindowElement<T, TBase>::GetWindow() const
{
	return CWindow(m_hWnd);
}

template <typename T, typename TBase>
IUOLMessengerCommandPtr CWindowElement<T, TBase>::GetCommand(const CString& strCommand)
{
	return IUOLMessengerCommandPtr();
}

template <typename T, typename TBase>
BOOL CWindowElement<T, TBase>::OnActivate(BOOL bActivate)
{
	m_pCurrentBackground = (bActivate) ? m_pActiveBackground.get() : m_pInactiveBackground.get();

	if (IsWindow())
	{
		InvalidateRect(NULL, FALSE);
	}	

	return FALSE;
}

template <typename T, typename TBase>
CRgnHandle  CWindowElement<T, TBase>::GetClippingRgn(const CRect& rectWindow) const
{
	return CRgnHandle();
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::SetBackground(CBackgroundElementPtr pBackGround)
{
	SetBackground(pBackGround, pBackGround);
}

template <typename T, typename TBase>
void CWindowElement<T, TBase>::SetBackground(CBackgroundElementPtr pActiveBackground, CBackgroundElementPtr pInactiveBackground)
{	
	m_pInactiveBackground = pInactiveBackground;
	m_pActiveBackground = pActiveBackground;	

	m_pCurrentBackground = m_pActiveBackground.get();	

	if (IsWindow())
	{
		InvalidateRect(NULL, TRUE);
		UpdateWindow();
	}
}