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
#include "AccountButton.h"
#include "../controller/UIMApplication.h"
#include "../resource.h"

#include "../controller/UIManager.h"
#include "skin/ProtocolImageElement.h"

#include <commands/changeavatarcommand.h>
#include <commands/changenicknamecommand.h>


CAccountButton::CAccountButton(HWND hWndParent, IUOLMessengerAccountPtr  pAccount) :
	CImageButton(),
	m_wndParent(hWndParent)
{
	SetAccount(pAccount);
	
	if (pAccount != NULL)
	{
        CString strState;
		
		strState.LoadString(IDS_STATUS_OFFLINE);
        SetTip(pAccount->GetUserId() + " - " + strState);
		
		SetWidth(20);
	}
}


CAccountButton::~CAccountButton()
{
	SetAccount(IUOLMessengerAccountPtr());
}


void CAccountButton::SetParent(HWND hWndParent)
{
	m_wndParent = hWndParent;
}


HWND CAccountButton::GetParent() const
{
	return m_wndParent;
}


void CAccountButton::SetAccount(IUOLMessengerAccountPtr pAccount)
{
	if (pAccount != NULL)
	{
		CUIMApplication::GetApplication()->GetAccountManager()->AddAccountListEventListener(this);

		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(pAccount, this);
	}
	else if (m_pAccount != NULL)
	{
		CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccountListEventListener(this);

		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(m_pAccount, this);
	}

	m_pAccount = pAccount;
}


IUOLMessengerAccountPtr CAccountButton::GetAccount()
{
	return m_pAccount;
}


void CAccountButton::UpdateIcon()
{
	if (m_pAccount != NULL)
	{
		IUOLMessengerImagesProtocol3Ptr pImagesProtocol3 = 
				ap_dynamic_cast<IUOLMessengerImagesProtocol3Ptr>(
						CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol());
		
		IUOLMessengerImagePtr pIcon;

		DWORD dwStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(m_pAccount);
		
		pIcon = pImagesProtocol3->GetStatusButtonImage(m_pAccount->GetProtocolId(), dwStatus);
		
		if (pIcon)
		{
			SetImage(pIcon);
		}
	}

	if (m_wndParent.IsWindow())
	{
		m_wndParent.InvalidateRect(NULL);
		m_wndParent.UpdateWindow();
	}
}

void CAccountButton::UpdateToolTip(DWORD dwStatus)
{
	CString strState;

	if (dwStatus == IUOLMessengerAccountConnectionManager::AS_ONLINE)
	{
        strState.LoadString(IDS_STATUS_ONLINE);
	}
	else if (dwStatus == IUOLMessengerAccountConnectionManager::AS_BUSY)
	{
        strState.LoadString(IDS_STATUS_BUSY);
	}
	else if ((dwStatus == IUOLMessengerAccountConnectionManager::AS_AWAY) ||
			(dwStatus == IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY))
	{
        strState.LoadString(IDS_STATUS_AWAY);
	}
	else if (dwStatus == IUOLMessengerAccountConnectionManager::AS_INVISIBLE)
	{
        strState.LoadString(IDS_STATUS_INVISIBLE);
	}
	else if (dwStatus == IUOLMessengerAccountConnectionManager::AS_OFFLINE)
	{
        strState.LoadString(IDS_STATUS_OFFLINE);
	}

	if (!strState.IsEmpty())
	{
        SetTip(m_pAccount->GetUserId() + " - " + strState);
	}
}


void CAccountButton::OnLButtonDown(const CPoint& ptClick)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	OpenContextMenu();
	CPoint ptMenu = ptClick;
	::ClientToScreen(m_wndParent, &ptMenu);
	m_pAccountMenu->ShowMenu(ptMenu);
	
	NotifyEventListeners(&CUOLMessengerImageButtonEventListener::OnLButtonDown, ptClick);
}


void CAccountButton::OnRButtonDown(const CPoint& ptClick)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	OpenContextMenu();
	CPoint ptMenu = ptClick;
	::ClientToScreen(m_wndParent, &ptMenu);
	m_pAccountMenu->ShowMenu(ptMenu);

	NotifyEventListeners(&CUOLMessengerImageButtonEventListener::OnRButtonDown, ptClick);
}


void CAccountButton::OnAccountProtocolChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldProtocolId)
{
	UpdateIcon();
}

void CAccountButton::OnAccountUserIdChanged(IUOLMessengerAccountPtr pAccount, const CString& strOldUserId)
{
	if (pAccount != NULL)
	{
		DWORD dwStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(pAccount);
		
		UpdateToolTip(dwStatus);
	}
}

void CAccountButton::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateIcon();
}

void CAccountButton::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateIcon();
}

void CAccountButton::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	UpdateIcon();
}

void CAccountButton::OnStatusChanged(IUOLMessengerAccountPtr pAccount)
{
	if (pAccount != NULL)
	{
		UpdateIcon();

		DWORD dwStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(pAccount);

		UpdateToolTip(dwStatus);
	}
}

void CAccountButton::OnCommand(IUOLMessengerContextMenuItem* pItem)
{
	if (m_pAccount)
	{
		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

		int nCommandId = pItem->GetCommand();

		// First validate operations...
		//
		if (nCommandId == ID_CHANGE_ONE_NICKNAME)
		{
			CUIMApplication::GetApplication()->ExecuteCommand(
					new CChangeNicknameCommand(m_hWnd, GetAccountNickname(), m_pAccount));
		}
		else if (nCommandId == ID_CHANGE_ONE_AVATAR)
		{
			CUIMApplication::GetApplication()->ExecuteCommand(
					new CChangeAvatarCommand(m_hWnd, m_pAccount));
		}
		else if (nCommandId == ID_SHOW_THIS_ACCOUNT_ON_PANEL)
		{
			IUOLMessengerUIManager6Ptr pUIManager6 = 
					ap_dynamic_cast<IUOLMessengerUIManager6Ptr>(
							CUIMApplication::GetApplication()->GetUIManager());
			
			if (pUIManager6)
			{
				pUIManager6->UpdateAccountInfoPanel(m_pAccount);
			}
		}
		// Now validate status changes...
		//
		else if (nCommandId == IUOLMessengerAccountConnectionManager::AS_OFFLINE)
		{
			pAccountConnectionManager->Disconnect(m_pAccount);
		}
		else
		{
			pAccountConnectionManager->SetStatus(m_pAccount, nCommandId);
	
			if (!pAccountConnectionManager->IsConnected(m_pAccount))
			{
				pAccountConnectionManager->Connect(m_pAccount);
			}
		}
	}
}


void CAccountButton::OpenContextMenu()
{
	m_pAccountMenu = new CContextMenu();
	
	if (m_pAccountMenu->CreateMenu())
	{
		// Add menu items...
		CString strText;
		
		IUOLMessengerAccountConnectionManager3Ptr pAccountConnectionManager3 = 
				ap_dynamic_cast<IUOLMessengerAccountConnectionManager3Ptr>(
						CUIMApplication::GetApplication()->GetAccountConnectionManager());
		
		if ( (pAccountConnectionManager3) && (m_pAccount) )
		{
			// First we add status change menu items...
			//
			CAtlList<int> listValidStatus;
			pAccountConnectionManager3->GetAccountValidStatus(m_pAccount, listValidStatus);
			
			for (POSITION listPos = listValidStatus.GetHeadPosition(); listPos; )
			{
				int iStatus = listValidStatus.GetNext(listPos);
				
				IUOLMessengerContextMenuItemPtr pMenuItem = 
						new CContextMenuItem(iStatus, GetMenuText(iStatus), this);
				
				m_pAccountMenu->AddItem(pMenuItem);
			}
			
			// Now we add operation menu items...
			//
			CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(m_pAccount->GetProtocol());
			
			if ( (pProtocol5) && ( (pProtocol5->HasUserAliasSupport()) || 
					(pProtocol5->HasUserDisplayIconSupport()) ) )
			{
				m_pAccountMenu->AddSeparator();
				
				if (pProtocol5->HasUserAliasSupport())
				{
					IUOLMessengerContextMenuItemPtr pMenuItem = 
							new CContextMenuItem(ID_CHANGE_ONE_NICKNAME, 
									GetMenuText(ID_CHANGE_ONE_NICKNAME), this);
					
					m_pAccountMenu->AddItem(pMenuItem);
				}
				
				if (pProtocol5->HasUserDisplayIconSupport())
				{
					IUOLMessengerContextMenuItemPtr pMenuItem = 
							new CContextMenuItem(ID_CHANGE_ONE_AVATAR, 
									GetMenuText(ID_CHANGE_ONE_AVATAR), this);
					
					m_pAccountMenu->AddItem(pMenuItem);
				}
			}
			
			// Add the show this account item.
			//
			m_pAccountMenu->AddSeparator();
			
			IUOLMessengerContextMenuItemPtr pMenuItem = 
					new CContextMenuItem(ID_SHOW_THIS_ACCOUNT_ON_PANEL, 
							GetMenuText(ID_SHOW_THIS_ACCOUNT_ON_PANEL), this);
			
			m_pAccountMenu->AddItem(pMenuItem);
		}
	}
}


CString CAccountButton::GetMenuText(int iMenuCmdId)
{
	CString strMenuText;
	
	UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	switch (iMenuCmdId)
	{
		case ID_SHOW_THIS_ACCOUNT_ON_PANEL:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_SHOW_THIS_ACCOUNT_ON_PANEL"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_CHANGE_ONE_NICKNAME);
			}
			
		break;
		
		case ID_CHANGE_ONE_NICKNAME:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_CHANGE_ONE_NICKNAME"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_CHANGE_ONE_NICKNAME);
			}
			
		break;
		
		case ID_CHANGE_ONE_AVATAR:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_CHANGE_ONE_AVATAR"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_CHANGE_ONE_AVATAR);
			}
			
		break;
		
		case IUOLMessengerAccountConnectionManager::AS_ONLINE:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_STATUS_ONLINE"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_STATUS_ONLINE);
			}
			
		break;
		
		case IUOLMessengerAccountConnectionManager::AS_BUSY:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_STATUS_BUSY"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_STATUS_BUSY);
			}
			
		break;
		
		case IUOLMessengerAccountConnectionManager::AS_AWAY:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_STATUS_AWAY"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_STATUS_AWAY);
			}
			
		break;
		
		case IUOLMessengerAccountConnectionManager::AS_INVISIBLE:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_STATUS_INVISIBLE"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_STATUS_INVISIBLE);
			}
			
		break;
		
		case IUOLMessengerAccountConnectionManager::AS_OFFLINE:
			
			if (pStringLoader)
			{
				strMenuText = pStringLoader->GetValue(_T("uol.resource.IDS_STATUS_OFFLINE"));
			}
			
			if (strMenuText.IsEmpty())
			{
				strMenuText.LoadString(IDS_STATUS_OFFLINE);
			}
			
		break;
	}
	
	return strMenuText;
}


CString CAccountButton::GetAccountNickname()
{
	CString strNickname = "";
	
	if (m_pAccount) 
	{
		strNickname = m_pAccount->GetNickName();
		
		if (strNickname.IsEmpty())
		{
			strNickname = m_pAccount->GetUserId();
		}
	}
	
	return strNickname;
}

