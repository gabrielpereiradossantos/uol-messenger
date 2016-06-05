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
#include "LowerChatContainer.h"
#include "ReadyForChatListCtrlBuilder.h"
#include "../TitlePanelBuilder.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/uimanager.h"


CLowerChatContainer::~CLowerChatContainer()
{
}


LRESULT CLowerChatContainer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CreateTitlePanel();
    CreateReadyForChatListCtrl();
    
    return 0L;
}


void CLowerChatContainer::OnDestroy()
{
    if (m_pReadyForChatListCtrl)
    {
        m_pReadyForChatListCtrl->Finalize();
    }
}


void CLowerChatContainer::OnSize(UINT nType, CSize size)
{
    UpdateLayout(size);
}


void CLowerChatContainer::UpdateLayout()
{
    CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());
    
    UpdateLayout(size);
}


void CLowerChatContainer::UpdateLayout(CSize size)
{
    CSize titleSize;
    m_pTitlePanel->UpdateLayout(size);
    m_pTitlePanel->GetMinSize(titleSize);

    int x = 0;
    int y = titleSize.cy;
    int cx = size.cx;
    int cy = size.cy - titleSize.cy;

    m_pReadyForChatListCtrl->SetWindowPos(NULL, x, y, cx, cy,  SWP_NOZORDER);
}


void CLowerChatContainer::CreateTitlePanel()
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
                CString strFormat = pStringLoader->GetValue("uol.resource.IDS_CHAT_READY_FOR_CHAT_LIST_TITLE");
                CString strName;
                strName.Format(strFormat, m_pConversation->GetAccount()->GetNickName(),
                    m_pConversation->GetAccount()->GetUserId());
                m_pTitlePanel->SetText(strName);

                DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
                CRect rect = CRect(0, 0, 0, 0);

                m_pTitlePanel->Create(m_hWnd, rect, NULL, dwStyle);
            }
        }
    }
}


void CLowerChatContainer::CreateReadyForChatListCtrl()
{
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
    IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__ReadyForChatListCtrlBuilderInstance");

	if (pElement)
    {
        CReadyForChatListCtrlBuilderPtr pBuilder;
        pBuilder = ap_dynamic_cast<CReadyForChatListCtrlBuilderPtr>(pElement);

        if (pBuilder)
        {
            m_pReadyForChatListCtrl = ap_dynamic_cast<CReadyForChatListCtrlPtr>(pBuilder->CreateElement());
        }

        if (m_pReadyForChatListCtrl.get())
        {
            m_pReadyForChatListCtrl->SetChatKey(m_pConversation->GetChatKey());

            IUOLMessengerFacade* pApp = CUIMApplication::GetApplication();
            m_pReadyForChatListCtrl->SetConversation(m_pConversation);
            m_pReadyForChatListCtrl->Init(pApp->GetContactList());

            CRect rect = CRect(0, 0, 0, 0);
            m_pReadyForChatListCtrl->Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        }
    }
}


CTitlePanelPtr CLowerChatContainer::GetTitlePanel()
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


CReadyForChatListCtrlPtr CLowerChatContainer::GetReadyForChatListCtrl()
{
    if (m_pReadyForChatListCtrl.get())
    {
        return m_pReadyForChatListCtrl;
    }
    else
    {
        return NULL;
    }
}


void CLowerChatContainer::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
    m_pConversation = pConversation;
}

