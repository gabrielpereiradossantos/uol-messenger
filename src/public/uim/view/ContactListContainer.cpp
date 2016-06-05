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


#include "stdafx.h"
#include "ContactListContainer.h"
#include "../controller/UIMApplication.h"
#include "../controller/uimanager.h"
#include "FindContactPanelBuilder.h"


CContactListContainer::~CContactListContainer()
{
}


LRESULT CContactListContainer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CUIMApplication::GetApplication()->GetAccountManager()->AddAccountListEventListener(this);

	// Register as accounts observer...
	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(listAccounts);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
		pAccountConnectionManager->RegisterAccountObserver(pAccount, this);
	}
    
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

    m_pContactListCtrl = ap_dynamic_cast<CContactListCtrlPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__ContactListInstance"));

	CreateFindContactPanel();
	CreateContactListCtrl();

	m_pContactListCtrl->SetFindContactPanel(m_pFindContactPanel);

    return 0L;
}


void CContactListContainer::OnDestroy()
{
	if (m_pContactListCtrl.get())
	{
		// Unregister as contact list settings event listener
		CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(m_pContactListCtrl.get());

		m_pContactListCtrl->Finalize();
	}
}


void CContactListContainer::OnSize(UINT nType, CSize size)
{
    UpdateLayout(size);
}


void CContactListContainer::UpdateLayout()
{
    CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());
    
    UpdateLayout(size);
}


void CContactListContainer::UpdateLayout(CSize size)
{
    CSize sizeFindContactPanel(0, 0);

	int y = 0;	

	if ((m_pFindContactPanel) && (m_pFindContactPanel->IsWindow()) && (m_pFindContactPanel->IsWindowVisible()))
	{
        m_pFindContactPanel->GetMinSize(sizeFindContactPanel);

        m_pFindContactPanel->SetWindowPos(NULL, 0, y, size.cx, sizeFindContactPanel.cy,  SWP_NOZORDER);
        y += sizeFindContactPanel.cy;
	}

    if ((m_pContactListCtrl) && (m_pContactListCtrl->IsWindow()))
	{
        m_pContactListCtrl->SetWindowPos(NULL, 0, y, size.cx, size.cy - sizeFindContactPanel.cy,  SWP_NOZORDER);
    }
}


void CContactListContainer::CreateFindContactPanel()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__FindContactPanelBuilderInstance");

	if (pElement)
	{
		CFindContactPanelBuilderPtr pBuilder;
		pBuilder = ap_dynamic_cast<CFindContactPanelBuilderPtr>(pElement);
		if (pBuilder)
		{
			m_pFindContactPanel = ap_dynamic_cast<CFindContactPanelPtr>(pBuilder->CreateElement());
			if (m_pFindContactPanel != NULL)
			{
				CRect rectPanel = CRect(CPoint(0, 0), CSize(0, 0));
				m_pFindContactPanel->Create(m_hWnd, rectPanel, NULL, WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
			}
		}
	}
}


void CContactListContainer::CreateContactListCtrl()
{
	if (m_pContactListCtrl != NULL)
	{
		IUOLMessengerFacade* pApp = CUIMApplication::GetApplication();
		m_pContactListCtrl->Init(pApp->GetContactList());

		CRect rect = CRect(CPoint(0, 0), CSize(20, 100));
		m_pContactListCtrl->Create(m_hWnd, rect, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}
}


CContactListCtrlPtr	CContactListContainer::GetContactListCtrl()
{
    return m_pContactListCtrl;
}


CFindContactPanelPtr CContactListContainer::GetFindContactPanel()
{
    return m_pFindContactPanel;
}


void CContactListContainer::OnConnected(IUOLMessengerAccountPtr pAccount)
{
    if (CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount() > 0)
    {
        //Show Find Contact Panel
		if (!m_pFindContactPanel->IsWindowVisible())
		{
			m_pFindContactPanel->ClearText();
			m_pFindContactPanel->ShowWindow(SW_SHOW);
			UpdateLayout();
		}
    }
}


void CContactListContainer::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
    if (CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount() == 0)
    {
        //Hide Find Contact Panel
        m_pFindContactPanel->ClearText();
		m_pFindContactPanel->ShowWindow(SW_HIDE);
        UpdateLayout();
    }
}


LRESULT CContactListContainer::OnFindContactPanelEscPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pFindContactPanel && m_pContactListCtrl)
	{
		m_pFindContactPanel->ClearText();

		m_pContactListCtrl->SetFocus();
        m_pContactListCtrl->SelectItem(1);
	}

	return 0L;
}
