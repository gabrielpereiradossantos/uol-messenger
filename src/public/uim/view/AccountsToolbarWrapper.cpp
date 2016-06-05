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
 
#include "StdAfx.h"
#include "AccountsToolbarWrapper.h"


CAccountsToolbarWrapper::CAccountsToolbarWrapper(CUOLToolBarPtr pAccountsToolbar) : 
		m_pAccountsToolbar(pAccountsToolbar)
{
}


CAccountsToolbarWrapper::~CAccountsToolbarWrapper(void)
{
}


BOOL CAccountsToolbarWrapper::Initialize()
{
	if (m_pAccountsToolbar.get())
	{
		m_pAccountsToolbar->AddButtonsEventListener(this);
		
		return TRUE;
	}
	
	return FALSE;
}


void CAccountsToolbarWrapper::Finalize()
{
	if (m_pAccountsToolbar.get())
	{
		m_pAccountsToolbar->RemoveButtonsEventListener(this);
	}
	
	m_accountButtonMap.RemoveAll();
}


void CAccountsToolbarWrapper::AddAccountButton(IUOLMessengerAccountPtr pAccount)
{
	if (m_pAccountsToolbar.get())
	{
		// Add account button
		CAccountButtonPtr pAccountButton = new CAccountButton(*m_pAccountsToolbar.get(), pAccount);
		pAccountButton->UpdateIcon();
		
		m_accountButtonMap.SetAt(pAccount, pAccountButton);
		
		pAccountButton->AddEventListener(this);
		
		m_pAccountsToolbar->AddExternalButton(pAccountButton);
	}
}


void CAccountsToolbarWrapper::RemoveAccountButton(IUOLMessengerAccountPtr pAccount)
{
	if (m_pAccountsToolbar.get())
	{
		CAccountButtonMap::CPair* pPair = m_accountButtonMap.Lookup(pAccount);
		
		if (pPair)
		{
			pPair->m_value->RemoveEventListener(this);
			
			m_pAccountsToolbar->RemoveExternalButton(pPair->m_value);

			m_accountButtonMap.RemoveKey(pPair->m_key);
			
			m_pAccountsToolbar->InvalidateRect(NULL);
			m_pAccountsToolbar->UpdateWindow();
		}
	}
}


/*
void CAccountsToolbarWrapper::GetMinSize(CSize& size)
{
	size = CSize(0, m_cy);
}


CToolTipElement CAccountsToolbarWrapper::GetSelectedControl(CPoint& pos)
{
	CToolTipElement toolTipElement;
	CImageButtonPtr pImageButton = CheckAccountButtonClick(pos);

	if (pImageButton != NULL)
	{
        toolTipElement.SetToolTipText(pImageButton->GetTip());
	}

	return toolTipElement;
}
*/


void CAccountsToolbarWrapper::OnLButtonDown(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	CAccountButtonPtr pAccountButton = GetAccountButtonFromImageButtonPtr(pButton);
	
	if (pAccountButton != NULL)
	{
		NotifyEventListeners(&CAccountsToolbarEventListener::OnLButtonDownAccountButton, ptClick, pAccountButton);
	}
}


void CAccountsToolbarWrapper::OnRButtonDown(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	CAccountButtonPtr pAccountButton = GetAccountButtonFromImageButtonPtr(pButton);
	
	if (pAccountButton != NULL)
	{
		NotifyEventListeners(&CAccountsToolbarEventListener::OnRButtonDownAccountButton, ptClick, pAccountButton);
	}
}


void CAccountsToolbarWrapper::AddEventListener(CAccountsToolbarEventListener* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listEventListener.Find(pListener);
		if (pos == NULL)
		{
			m_listEventListener.AddTail(pListener);
		}
	}
}

void CAccountsToolbarWrapper::RemoveEventListener(CAccountsToolbarEventListener* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listEventListener.Find(pListener);
		if (pos != NULL)
		{
			m_listEventListener.RemoveAt(pos);
		}
	}
}


/*
void CAccountsToolbarWrapper::DoPaint(CDCHandle& dcHandle)
{
	__super::DoPaint(dcHandle);

	DrawAccountButtons(dcHandle);
}


void CAccountsToolbarWrapper::UpdateLayout(BOOL bResizeBars)
{
	__super::UpdateLayout(bResizeBars);

	InvalidateRect(NULL);
}


void CAccountsToolbarWrapper::OnLButtonDown(UINT nFlags, CPoint& ptClick)
{
	CAccountButtonPtr pAccountButton = CheckAccountButtonClick(ptClick);
	if (pAccountButton != NULL)
	{
		pAccountButton->OnLButtonDown(ptClick);

		NotifyEventListeners(&CAccountsToolbarWrapperEventListener::OnLButtonDownAccountButton, ptClick, pAccountButton);
	}
}

void CAccountsToolbarWrapper::OnRButtonDown(UINT nFlags, CPoint& ptClick)
{
	CAccountButtonPtr pAccountButton = CheckAccountButtonClick(ptClick);
	if (pAccountButton != NULL)
	{
		pAccountButton->OnRButtonDown(ptClick);

		NotifyEventListeners(&CAccountsToolbarWrapperEventListener::OnRButtonDownAccountButton, ptClick, pAccountButton);
	}
}
*/


CAccountButtonPtr CAccountsToolbarWrapper::GetAccountButtonFromImageButtonPtr(
		IUOLMessengerImageButton* pButton)
{
	CAccountButtonPtr pAccountButton;
	
	for (POSITION listPos = m_accountButtonMap.GetStartPosition(); listPos; )
	{
		CAccountButtonMap::CPair* pPair = m_accountButtonMap.GetNext(listPos);
		
		if (pPair->m_value.get() == pButton)
		{
			pAccountButton = pPair->m_value;
			break;
		}
	}
	
	return pAccountButton;
}


void CAccountsToolbarWrapper::NotifyEventListeners(void (CAccountsToolbarEventListener::*pfnCallback)(const CPoint& ptClick, CAccountButtonPtr pButton), const CPoint& ptClick, CAccountButtonPtr pButton)
{
	CAccountsToolbarEventListener* pListener;
	
	for (POSITION pos = m_listEventListener.GetHeadPosition(); pos; )
	{
		pListener = m_listEventListener.GetNext(pos);
		(pListener->*pfnCallback)(ptClick, pButton);
	}
}


/*
void CAccountsToolbarWrapper::DrawAccountButtons(CDCHandle& dc)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	int x = rectClient.Width() - PM_HBORDER;
	int y = PM_VBORDER;

	int nIconSpacing = GetAccountButtonsSpacing();

	for (POSITION pos = m_listAccountButtons.GetTailPosition(); pos;)
	{
		CAccountButtonPtr pAccountButton = m_listAccountButtons.GetPrev(pos);
		ATLASSERT(pAccountButton != NULL);

		IUOLMessengerImagePtr pImage = pAccountButton->GetImage();

		if (pImage != NULL)
		{
			int w = pImage->GetWidth();
			int h = pImage->GetHeight();

			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();

			pImage->Draw(dc, 
					x - w, y, 
					w, h, 
					pStrategy);

			x -= (w + nIconSpacing);
		}
	}
}


int CAccountsToolbarWrapper::GetAccountButtonsSpacing() const
{
	return 5;
}


CAccountButtonPtr CAccountsToolbarWrapper::CheckAccountButtonClick(const CPoint& ptClick)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	int x = rectClient.Width() - PM_HBORDER;
	int y = PM_VBORDER;
	int nIconSpacing = GetAccountButtonsSpacing();
	
	int nIconWidth, nIconHeight;
	CRect rectTest;
	for (POSITION pos = m_listAccountButtons.GetTailPosition(); pos;)
	{
		CAccountButtonPtr pAccountButton = m_listAccountButtons.GetPrev(pos);
		ATLASSERT(pAccountButton != NULL);

		IUOLMessengerImagePtr pImage = pAccountButton->GetImage();

		if (pImage != NULL)
		{
			nIconWidth = pImage->GetWidth();
			nIconHeight = pImage->GetHeight();

			rectTest = CRect(CPoint(x - nIconWidth, y), CSize(nIconWidth, nIconHeight));

			if (rectTest.PtInRect(ptClick))
			{
				return pAccountButton;
			}

			x -= (nIconWidth + nIconSpacing);
		}
	}

	return CAccountButtonPtr();
}


void CAccountsToolbarWrapper::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);	

	GetParameterValue(m_cy, pMapParams, "Height");
}


CString CAccountsToolbarWrapper::GetName() const
{
	return "__AccountsToolbarWrapperInstance";
}
*/
