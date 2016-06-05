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

#include "tooltip/ShowToolTip.h"
#include "tooltip/ToolTipElement.h"

#include "skin/windowelement.h"
#include "ImageButton.h"
#include "MultiStateImageButton.h"
#include "FindPanelButtonBuilder.h"

#include "../view/image/Image.h"
#include "skin/fontelement.h"

#include <interfaces/IUOLMessengerImageButton.h>
#include <interfaces/IUOLMessengerImageButtonContainer.h>
#include <interfaces/IUOLMessengerMultiStateImageButton.h>
#include <interfaces/IUOLMessengerFindPanel.h>


template <typename T, typename TBase>
class CImageButtonContainer : public CWindowElement<T, TBase >,
								public IUOLMessengerImageButtonContainer,
								public CShowToolTip<CImageButtonContainer<T, TBase> >
{
public:
	CImageButtonContainer(void);
	virtual ~CImageButtonContainer(void);

	virtual IUOLMessengerImageButtonPtr CreateExternalButton() const;
	virtual void AddExternalButton(IUOLMessengerImageButtonPtr pImageButton);
	virtual void RemoveExternalButton(IUOLMessengerImageButtonPtr pImageButton);

	virtual IUOLMessengerMultiStateImageButtonPtr CreateExternalMultiStateButton() const;
	virtual void AddExternalMultiStateButton(IUOLMessengerMultiStateImageButtonPtr pImageButton);
	virtual void RemoveExternalMultiStateButton(IUOLMessengerMultiStateImageButtonPtr pImageButton);

	CToolTipElement GetSelectedControl(CPoint& pos);
protected:
	typedef CWindowElement<T, TBase > _Base;

	BEGIN_MSG_MAP(CImageButtonContainer)		
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)		
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(_Base)
		CHAIN_MSG_MAP(CShowToolTip<CImageButtonContainer>)
	END_MSG_MAP()

	LRESULT OnSetCursor(HWND, UINT, UINT);
	void OnMouseMove(UINT nFlags, CPoint& ptPos);
	void OnLButtonDown(UINT nFlags, CPoint& ptClick);
	void OnRButtonDown(UINT nFlags, CPoint& ptClick);
	void OnLButtonUp(UINT nFlags, CPoint& ptClick);
	void OnLButtonDblClick(UINT nFlags, CPoint& ptClick);
	LRESULT OnCreate(LPCREATESTRUCT lpCreatestruct);
	void OnDestroy();
	void OnTimer(UINT, TIMERPROC);

	virtual void DoPaint(CDCHandle& dc);
	virtual void DrawImages(CDCHandle& dc);	
	virtual void DrawButton(CDCHandle& dc, CImageButtonBasePtr&	pButton, int x, int y, int cx, int cy, CImageButton::MouseState state);
	virtual void OnLButtonDown(const CPoint& ptClick, CImageButtonBasePtr pButton);
	virtual void OnRButtonDown(const CPoint& ptClick, CImageButtonBasePtr pButton);
	virtual void OnLButtonUp(const CPoint& ptClick, CImageButtonBasePtr pButton);
	virtual void OnLButtonDblClick(const CPoint& ptClick, CImageButtonBasePtr pButton);

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual void GetMinSize(CSize& size);
	
	virtual BOOL HitTest(const CPoint& pt, int& nTest) const;

	virtual void AddButtonsEventListener(CUOLMessengerImageButtonEventListener2* pListener);
	virtual void RemoveButtonsEventListener(CUOLMessengerImageButtonEventListener2* pListener);
	virtual void RemoveAllButtonsEventListener();

	// Classe auxiliar que visa dar suporte à pintura dos botões.
	// Cada botão terá o seu estado atualizado pelas mensagens
	// WM_*MOUSE.	
	struct CAddElementHelper : public IElement
	{
		CAddElementHelper(IElementPtr pElement) : 
			m_state(CImageButton::MouseOut), 
			m_pButton() 
			{
				CImageButtonBuilderPtr pBtnBuilder = ap_dynamic_cast<CImageButtonBuilderPtr>(pElement);
				//ATLASSERT(pBtnBuilder.get());
				
				if (pBtnBuilder)
				{
					m_pButton = ap_dynamic_cast<CImageButtonBasePtr>(pBtnBuilder->CreateElement());
					ATLASSERT(m_pButton.get());
					
					m_pButtonElement = ap_dynamic_cast<IElementPtr>(m_pButton);
					ATLASSERT(m_pButtonElement.get());
				}
				else
				{
					CFindPanelButtonBuilderPtr pBtnBuilder = ap_dynamic_cast<CFindPanelButtonBuilderPtr>(pElement);
					ATLASSERT(pBtnBuilder.get());
					
					m_pButton = ap_dynamic_cast<CImageButtonBasePtr>(pBtnBuilder->CreateElement());
					ATLASSERT(m_pButton.get());
					
					m_pButtonElement = ap_dynamic_cast<IElementPtr>(m_pButton);
					ATLASSERT(m_pButtonElement.get());
				}
			}

		CAddElementHelper(IUOLMessengerImageButtonPtr pImageButton) : 
			m_state(CImageButton::MouseOut), 
			m_pButton() 
			{
				m_pButton = ap_dynamic_cast<CImageButtonBasePtr>(pImageButton);
				ATLASSERT(m_pButton.get());
			}

		virtual CString GetName() const
		{
			return m_pButtonElement->GetName();
		}
		
		virtual void AddElement(IElementPtr pElement)
		{
			m_pButtonElement->AddElement(pElement);
		}

		virtual void RemoveElement(IElementPtr pElement)
		{
			m_pButtonElement->RemoveElement(pElement);
		}

		virtual void OnElementBuilt()
		{
			m_pButtonElement->OnElementBuilt();
		}

		IElementPtr m_pButtonElement;
		CImageButtonBasePtr m_pButton;
		CImageButton::MouseState	m_state;
	
	}; MAKEAUTOPTR(CAddElementHelper);

	// CElement override
	virtual void AddElement(IElementPtr pElement);
	virtual CAddElementHelperPtr GetImageButton(CPoint& pt, CImageButton::MouseState mouseState)
	{
		BOOL bPtInImage;
		CRect rectImage;
		int x = GetFirstButtonPosition();
		int w = 0;
		int h = 0;

		CSize size;
		GetMinSize(size);

		CImageButtonContainer<T, TBase>::CAddElementHelperPtr pHelper;

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
		{
			CAddElementHelperPtr pHTmp = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
			CImageButtonBasePtr	pButton = pHTmp->m_pButton;
			ATLASSERT(pButton != NULL);				

			if (pButton->IsVisible()==FALSE) 
			{
				continue;
			}

			IUOLMessengerImagePtr pImage = pButton->GetImage();			

			if (pImage)
			{
				h = pImage->GetHeight();

				w = pButton->GetWidth();					

				rectImage.left = x + GetButtonOffset(pButton);
				rectImage.top = (size.cy - h) / 2;
				rectImage.right = rectImage.left + w;
				rectImage.bottom = rectImage.top + size.cy;

				bPtInImage = rectImage.PtInRect(pt);

				CImageButton::MouseState stateCurrent = pHTmp->m_state;

				if (bPtInImage)
				{
					m_bDirty = TRUE;
					pHTmp->m_state = mouseState;

					pHelper = pHTmp;			
				}
				else
				{
					pHTmp->m_state = CImageButton::MouseOut;
				}

				if (stateCurrent != pHTmp->m_state)
				{
					InvalidateRect(&rectImage, FALSE);
				}
			}
			
			x = GetNextButtonPosition(x, pButton);
		}

		return pHelper;
	}

	// Default is Left to Right
	virtual int GetFirstButtonPosition();
	virtual int GetNextButtonPosition(int x, CImageButtonBasePtr& pButton);
	virtual int GetButtonOffset(CImageButtonBasePtr& pButton);

private:

protected:
	BOOL					m_bDirty;
	int						m_nSpacement;
	int						m_cy;
	CString					m_strHOrientation;

	enum _TIMER_ID { TIMER_ID_UPDATE_BUTTONS };

	static CString HORIZONTAL_ORIENTATION_LEFT;
	static CString HORIZONTAL_ORIENTATION_RIGHT;

private:
    CToolTipElement			m_toolTipElement;
};

template <typename T, typename TBase>
CToolTipElement CImageButtonContainer<T, TBase>::GetSelectedControl(CPoint& pos)
{
	return m_toolTipElement;
}

template <typename T, typename TBase>
IUOLMessengerImageButtonPtr CImageButtonContainer<T, TBase>::CreateExternalButton() const
{
	return new CImageButton(m_hWnd);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::AddExternalButton(IUOLMessengerImageButtonPtr pImageButtonPtr)
{
	CAddElementHelperPtr pHelper = new CAddElementHelper(pImageButtonPtr);

	__super::AddElement(pHelper);

	Invalidate();
	UpdateWindow();
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::RemoveExternalButton(IUOLMessengerImageButtonPtr pImageButtonPtr)
{
	BOOL bUpdate = FALSE;

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHelper = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pButton = pHelper->m_pButton;
		ATLASSERT(pButton != NULL);

		if (pButton.get() == pImageButtonPtr.get())
		{
			__super::RemoveElement(pHelper);
			
			bUpdate = TRUE;
			break;
		}
	}

	if (bUpdate)
	{
		Invalidate();
		UpdateWindow();
	}
}

template <typename T, typename TBase>
IUOLMessengerMultiStateImageButtonPtr CImageButtonContainer<T, TBase>::CreateExternalMultiStateButton() const
{
	return new CMultiStateImageButton(m_hWnd);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::AddExternalMultiStateButton(IUOLMessengerMultiStateImageButtonPtr pImageButton)
{
	AddExternalButton(pImageButton);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::RemoveExternalMultiStateButton(IUOLMessengerMultiStateImageButtonPtr pImageButton)
{
	RemoveExternalButton(pImageButton);
}

template <typename T, typename TBase>
CImageButtonContainer<T, TBase>::CImageButtonContainer(void) : 
	m_nSpacement(0), 
	m_cy(0), 
	m_bDirty(FALSE)
{
}

template <typename T, typename TBase>
CImageButtonContainer<T, TBase>::~CImageButtonContainer(void)
{
}


template <typename T, typename TBase>
LRESULT CImageButtonContainer<T, TBase>::OnSetCursor(HWND, UINT nHitTest, UINT nMessage)
{
	CPoint pt = ::GetMessagePos();	

	CRect rect;
	GetWindowRect(&rect);

	if ((WM_MOUSEMOVE == nMessage) && (FALSE == rect.PtInRect(pt)))
	{   
		ScreenToClient(&pt);
		CAddElementHelperPtr pHelper = GetImageButton(pt, CImageButton::MouseOut);			
	}

	SetMsgHandled(FALSE);

	return 0L;
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnMouseMove(UINT nFlags, CPoint& ptPos)
{
	T* pT = (T*)this;

	if (pT != NULL)
	{
		CAddElementHelperPtr pHelper = GetImageButton(ptPos, CImageButton::MouseOver);	

		if (pHelper && pHelper->m_pButton)
		{
			m_toolTipElement.SetToolTipText(pHelper->m_pButton->GetTip());
		}
		else
		{
			m_toolTipElement.SetToolTipText("");

            m_toolTipElement = pT->GetSelectedControl(ptPos);
		}
	}

	SetMsgHandled(FALSE);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnLButtonDown(UINT nFlags, CPoint& ptClick)
{
	CAddElementHelperPtr pHelper = GetImageButton(ptClick, CImageButton::MouseClick);	
	
	if (pHelper != NULL)
	{
		CImageButtonBasePtr pButton = pHelper->m_pButton;

		pButton->OnLButtonDown(ptClick);

		OnLButtonDown(ptClick, pButton);
	}
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnRButtonDown(UINT nFlags, CPoint& ptClick)
{
	CAddElementHelperPtr pHelper = GetImageButton(ptClick, CImageButton::MouseClick);	
	
	if (pHelper != NULL)
	{
		CImageButtonBasePtr pButton = pHelper->m_pButton;

		pButton->OnRButtonDown(ptClick);

		OnRButtonDown(ptClick, pButton);
	}
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnLButtonUp(UINT nFlags, CPoint& ptClick)
{
	CAddElementHelperPtr pHelper = GetImageButton(ptClick, CImageButton::MouseOver);	

	if (pHelper != NULL)
	{
		CImageButtonBasePtr pButton = pHelper->m_pButton;

		pButton->OnLButtonUp(ptClick);

		OnLButtonUp(ptClick, pButton);
	}
}


template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnLButtonDblClick(UINT nFlags, CPoint& ptClick)
{
	CAddElementHelperPtr pHelper = GetImageButton(ptClick, CImageButton::MouseClick);	
	
	if (pHelper != NULL)
	{
		CImageButtonBasePtr pButton = pHelper->m_pButton;

		pButton->OnLButtonDown(ptClick);

		OnLButtonDblClick(ptClick, pButton);
	}	
}
	 
template <typename T, typename TBase>
LRESULT CImageButtonContainer<T, TBase>::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	SetTimer(TIMER_ID_UPDATE_BUTTONS, 200);

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHTmp = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pButton = pHTmp->m_pButton;
		ATLASSERT(pButton != NULL);

		pButton->SetParent(m_hWnd);
	}

	SetMsgHandled(FALSE);

	return 0L;
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnDestroy()
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHTmp = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pButton = pHTmp->m_pButton;
		ATLASSERT(pButton != NULL);
		
		pButton->Release();
	}
	
	RemoveAllButtonsEventListener();

	KillTimer(TIMER_ID_UPDATE_BUTTONS);

	SetMsgHandled(FALSE);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnTimer(UINT nId, TIMERPROC)
{
	switch (nId)
	{
	case TIMER_ID_UPDATE_BUTTONS:
	{
		CPoint pt(::GetMessagePos());	

		CRect rect;
		GetWindowRect(&rect);		

		if ((m_bDirty) && (FALSE == rect.PtInRect(pt)))
		{
			ScreenToClient(&pt);
			CAddElementHelperPtr pHelper = GetImageButton(pt, CImageButton::MouseOut);	

			m_bDirty = FALSE;;
		}		
		return;
	}		
	}

	SetMsgHandled(FALSE);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::DoPaint(CDCHandle& dc)
{
	DrawImages(dc);	
}


template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::DrawImages(CDCHandle& dc)
{
	CRect rect;
	GetClientRect(&rect);

	CSize size;
	GetMinSize(size);

	int x = GetFirstButtonPosition();		
	int y = 0;
	

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CImageButtonContainer::CAddElementHelperPtr pHelper = ap_static_cast<CImageButtonContainer::CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pButton = pHelper->m_pButton;
		ATLASSERT(pButton != NULL);		

		if ( (pButton.get() == NULL) || (pButton->IsVisible() == FALSE) )
		{
			continue;
		}
		
		if (pButton->GetImage().get())
		{
			int nButtonWidth = pButton->GetWidth();
			IUOLMessengerImagePtr pImage = pButton->GetImage();

			int w = pImage->GetWidth();
			int h = pImage->GetHeight();

			CImageButton::MouseState state;
			
			if ((pButton->IsPressed()) && (pHelper->m_state != CImageButton::MouseOver))
			{
				state = (CImageButton::MouseState) min(CImageButton::MouseClick, (w / nButtonWidth) - 1);
			}
			else
			{
				state = (CImageButton::MouseState) min(pHelper->m_state, (w / nButtonWidth) - 1);
			}

			//int xSrc = nButtonWidth * state;

			y = (size.cy - h) / 2;

			DrawButton(dc, 
					   pButton, 
					   x + GetButtonOffset(pButton), y, 
					   nButtonWidth, h,
					   state);
		}
		else
		{
			IUOLMessengerFindPanelPtr pFindPanel = 
					ap_dynamic_cast<IUOLMessengerFindPanelPtr>(pButton);
			
			if (pFindPanel)
			{
				DrawButton(dc, 
						   pButton, 
						   x + GetButtonOffset(pButton), 0, 
						   pButton->GetWidth(), size.cy,
						   CImageButton::MouseOut);
			}
		}
		
		x = GetNextButtonPosition(x, pButton);
	}
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::DrawButton(CDCHandle& dc, 
												CImageButtonBasePtr& pButton, 
												int x, int y, 
												int cx, int cy, 
												CImageButton::MouseState state)
{
	pButton->Draw(dc,
			  x, y, 
			  cx, cy,
			  state);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnLButtonDown(const CPoint& ptClick, CImageButtonBasePtr pButton)
{
	
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnRButtonDown(const CPoint& ptClick, CImageButtonBasePtr pButton)
{
	
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnLButtonUp(const CPoint& ptClick, CImageButtonBasePtr pButton)
{
	
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::OnLButtonDblClick(const CPoint& ptClick, CImageButtonBasePtr pButton)
{
	
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	GetParameterValue(m_nSpacement, pMapParams, "ButtonSpacement");

	try
	{
		GetParameterValue(m_cy, pMapParams, "height");
	}
	catch(const CSkinException&)
	{
		m_cy = -1;
	}	
	
	try
	{
		GetParameterValue(m_strHOrientation, pMapParams, "hOrientation");
	}
	catch(const CSkinException&)
	{
		m_strHOrientation = HORIZONTAL_ORIENTATION_LEFT;
	}
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::GetMinSize(CSize& size)
{
	int nHeight = 0;
	int nWidth = 0;

	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHelper = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pButton = pHelper->m_pButton;
		ATLASSERT(pButton != NULL);

		/* verify if the button is visible */
		if(!pButton->IsVisible())
			continue;
	
		IUOLMessengerImagePtr pImage = pButton->GetImage();
		
		if (pImage)
		{
			int cy = pImage->GetHeight();
			int cx = pButton->GetWidth();

			nHeight = max(nHeight, cy);
			nWidth += (2 * m_nSpacement) + cx;
		}
		else
		{
			IUOLMessengerFindPanelPtr pFindPanel = 
					ap_dynamic_cast<IUOLMessengerFindPanelPtr>(pButton);
			
			if (pFindPanel)
			{
				nWidth += (2 * m_nSpacement) + pButton->GetWidth();
			}
		}
	}

	size = CSize(nWidth, (-1 != m_cy) ? m_cy : nHeight);
}



template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::AddButtonsEventListener(CUOLMessengerImageButtonEventListener2* pListener)
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHTmp = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));		
		CImageButtonBasePtr	pButton = pHTmp->m_pButton;
		ATLASSERT(pButton != NULL);

		pButton->AddEventListener(pListener);
	}	
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::RemoveButtonsEventListener(CUOLMessengerImageButtonEventListener2* pListener)
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHTmp = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pButton = pHTmp->m_pButton;
		ATLASSERT(pButton != NULL);

		pButton->RemoveEventListener(pListener);
	}
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::RemoveAllButtonsEventListener()
{
	m_listChilds.RemoveAll();
}

template <typename T, typename TBase>
BOOL CImageButtonContainer<T, TBase>::HitTest(const CPoint& pt, int& nTest) const
{
	return __super::HitTest(pt, nTest);
}

template <typename T, typename TBase>
void CImageButtonContainer<T, TBase>::AddElement(IElementPtr pElement)
{
	CAddElementHelperPtr pHelper = new CAddElementHelper(pElement);

	__super::AddElement(pHelper);
}


template <typename T, typename TBase>
int CImageButtonContainer<T, TBase>::GetFirstButtonPosition()
{
	return m_nSpacement;
}

template <typename T, typename TBase>
int CImageButtonContainer<T, TBase>::GetNextButtonPosition(int x, CImageButtonBasePtr& pButton)
{
	return x + pButton->GetWidth() + (2 * m_nSpacement);
}

template <typename T, typename TBase>
int CImageButtonContainer<T, TBase>::GetButtonOffset(CImageButtonBasePtr& /*pButton*/)
{
	if (m_strHOrientation.CompareNoCase(HORIZONTAL_ORIENTATION_RIGHT) == 0)
	{
		CRect rect;
		GetClientRect(&rect);

		CSize size;
		GetMinSize(size);

		return (rect.Width() - size.cx);
	}
	else
	{
		return 0;
	}
}


template <typename T, typename TBase>
CString CImageButtonContainer<T, TBase>::HORIZONTAL_ORIENTATION_LEFT = "left";

template <typename T, typename TBase>
CString CImageButtonContainer<T, TBase>::HORIZONTAL_ORIENTATION_RIGHT = "right";
