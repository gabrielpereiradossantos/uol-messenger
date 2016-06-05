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
#include "UpperChatContainer.h"
#include "ChatListCtrlBuilder.h"
#include "../TitlePanelBuilder.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/uimanager.h"


CUpperChatContainer::~CUpperChatContainer()
{
}


LRESULT CUpperChatContainer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CreateTitlePanel();
    CreateChatListCtrl();
    
    return 0L;
}


void CUpperChatContainer::OnDestroy()
{
    if (m_pChatListCtrl)
    {
        m_pChatListCtrl->Finalize();
    }
}


void CUpperChatContainer::OnSize(UINT nType, CSize size)
{
    UpdateLayout(size);
}


void CUpperChatContainer::UpdateLayout()
{
    CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());
    
    UpdateLayout(size);
}


void CUpperChatContainer::UpdateLayout(CSize size)
{
    CSize titleSize;
    m_pTitlePanel->UpdateLayout(size);
    m_pTitlePanel->GetMinSize(titleSize);

    int x = 0;
    int y = titleSize.cy;
    int cx = size.cx;
    int cy = size.cy - titleSize.cy;

    m_pChatListCtrl->SetWindowPos(NULL, x, y, cx, cy,  SWP_NOZORDER);
}


void CUpperChatContainer::CreateTitlePanel()
{
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
    IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__TitlePanelBuilderInstance");

    if (pElement)
    {
        CTitlePanelBuilderPtr pBuilder;
        pBuilder = ap_dynamic_cast<CTitlePanelBuilderPtr>(pElement);

        if (pBuilder)
        {
            m_pTitlePanel = ap_dynamic_cast<CTitlePanelPtr>(pBuilder->CreateElement());
            
            if (m_pTitlePanel)
            {
                UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
                CString strName = pStringLoader->GetValue("uol.resource.IDS_CHAT_PARTICIPANT_CHAT_LIST_TITLE");
                m_pTitlePanel->SetText(strName);

                DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
                CRect rect = CRect(0, 0, 0, 0);

                m_pTitlePanel->Create(m_hWnd, rect, NULL, dwStyle);
            }
        }
    }
}


void CUpperChatContainer::CreateChatListCtrl()
{
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
    IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__ChatListCtrlBuilderInstance");

	if (pElement)
    {
        CChatListCtrlBuilderPtr pBuilder;
        pBuilder = ap_dynamic_cast<CChatListCtrlBuilderPtr>(pElement);

        if (pBuilder)
        {
            m_pChatListCtrl = ap_dynamic_cast<CChatListCtrlPtr>(pBuilder->CreateElement());
        }

        if (m_pChatListCtrl.get())
        {
            m_pChatListCtrl->SetChatKey(m_pConversation->GetChatKey());

            IUOLMessengerFacade* pApp = CUIMApplication::GetApplication();
            m_pChatListCtrl->SetConversation(m_pConversation);
            m_pChatListCtrl->Init(pApp->GetContactList());

            CRect rect = CRect(0, 0, 0, 0);
            m_pChatListCtrl->Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        }
    }
}


CTitlePanelPtr CUpperChatContainer::GetTitlePanel()
{
    if (m_pTitlePanel.get())
    {
        return m_pTitlePanel;
    }
    else
    {
        return NULL;
    }
}


CChatListCtrlPtr CUpperChatContainer::GetChatListCtrl()
{
    if (m_pChatListCtrl.get())
    {
        return m_pChatListCtrl;
    }

    else
    {
        return NULL;
    }
}


void CUpperChatContainer::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
    m_pConversation = pConversation;
}

