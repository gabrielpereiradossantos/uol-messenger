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
#include "./UOLMessengerTrayWindow.h"
#include "../resource.h"

#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"
#include "skin/SystemTrayIconListElement.h"


CUOLMessengerTrayWindow::~CUOLMessengerTrayWindow()
{
}


HWND CUOLMessengerTrayWindow::Create(HWND hParent)
{
	m_dwCurrentStatus = IUOLMessengerAccountConnectionManager::AS_OFFLINE;
	HWND hwnd = __super::Create(hParent, NULL);

	return hwnd;
}

BOOL CUOLMessengerTrayWindow::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	m_pUOLMessengerFacade = pUOLMessengerFacade;

	return TRUE;
}

void CUOLMessengerTrayWindow::AddTrayWindowEventListener(CUOLMessengerTrayWindowEventListener* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listListeners.Find(pListener);

		if (pos == NULL)
		{
			m_listListeners.AddTail(pListener);
		}
	}
}

void CUOLMessengerTrayWindow::RemoveTrayWindowEventListener(CUOLMessengerTrayWindowEventListener* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listListeners.Find(pListener);

		if (pos)
		{
			m_listListeners.RemoveAt(pos);
		}
	}
}

void CUOLMessengerTrayWindow::OnStatusChanged(IUOLMessengerAccountPtr pAccount)
{
	int nAccountCount = m_pUOLMessengerFacade->GetAccountManager()->GetAccountListCount();
	int nStatusCount = 0;

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pUOLMessengerFacade->GetAccountManager()->GetAccountList(listAccounts);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pCurrentAccount = listAccounts.GetNext(pos);

		if (pAccountConnectionManager->GetStatus(pCurrentAccount) == pAccountConnectionManager->GetStatus(pAccount))
		{
			nStatusCount++;
		}
		else
		{
			break;
		}
	}

	DWORD dwNewStatus = pAccountConnectionManager->GetStatus(pAccount);

	// Change icon if all accounts user same status
	if ((nStatusCount == nAccountCount) /*&& (dwNewStatus != IUOLMessengerAccountConnectionManager::AS_OFFLINE)*/)
	{
		UINT nStatusTextResID;

		if (IUOLMessengerAccountConnectionManager::AS_ONLINE == dwNewStatus)
		{
			nStatusTextResID = IDS_STATUS_ONLINE;
		}
		else if (IUOLMessengerAccountConnectionManager::AS_AWAY == dwNewStatus)
		{
			nStatusTextResID = IDS_STATUS_AWAY;
		}
		else if (IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY == dwNewStatus)
		{
			nStatusTextResID = IDS_STATUS_IDLE;
		}
		else if (IUOLMessengerAccountConnectionManager::AS_BUSY == dwNewStatus)
		{
			nStatusTextResID = IDS_STATUS_BUSY;
		}
		else if (IUOLMessengerAccountConnectionManager::AS_OFFLINE == dwNewStatus)
		{
			nStatusTextResID = IDS_STATUS_OFFLINE;
		}
		else 
		{
			// (IUOLMessengerAccount::AS_INVISIBLE == dwNewStatus)
			nStatusTextResID = IDS_STATUS_INVISIBLE;
		}

		// Update tooltip...
		CString strStatusText;
		strStatusText.LoadString(nStatusTextResID);
		
		CString strTooltip;
		strTooltip = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
		strTooltip.Append(" - ");
		strTooltip.Append(strStatusText);
		SetTip(strTooltip);

		// Update icon
		m_iconCurrent.Detach();
		m_iconCurrent = GetStatusIcon((IUOLMessengerAccountConnectionManager::AccountStatus) dwNewStatus);

		SetIcon(m_iconCurrent);
		UpdateIcon();

		m_dwCurrentStatus = dwNewStatus;
	}
	else
	{
		UINT nStatusTextResID;

		nStatusTextResID = IDS_STATUS_PERSONALIZED;

		CString strStatusText;
		strStatusText.LoadString(nStatusTextResID);
		
		CString strTooltip;
		strTooltip = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
		strTooltip.Append(" - ");
		strTooltip.Append(strStatusText);
		SetTip(strTooltip);

		// Update icon
		m_iconCurrent.Detach();
		m_iconCurrent = GetStatusIcon(IUOLMessengerAccountConnectionManager::AS_PERSONALIZED);

		SetIcon(m_iconCurrent);
		UpdateIcon();

		m_dwCurrentStatus = IUOLMessengerAccountConnectionManager::AS_PERSONALIZED;
	}
}

void CUOLMessengerTrayWindow::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	OnStatusChanged(pAccount);
}

void CUOLMessengerTrayWindow::OnAddAccount(IUOLMessengerAccountPtr pAccount)
{
	CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(pAccount, this);
}

void CUOLMessengerTrayWindow::OnRemoveAccount(IUOLMessengerAccountPtr pAccount)
{
	CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(pAccount, this);
}

void CUOLMessengerTrayWindow::NotifyTrayWindowEventListeners(void (CUOLMessengerTrayWindowEventListener::*pEventHandler)(IUOLMessengerTrayWindow* pTrayWindow, const CPoint& ptClick), const CPoint& ptClick)
{
	CUOLMessengerTrayWindowEventListener* pListener;
	for (POSITION pos = m_listListeners.GetHeadPosition(); pos; )
	{
		pListener = m_listListeners.GetNext(pos);
		(pListener->*pEventHandler)(this, ptClick);
	}
}

LRESULT CUOLMessengerTrayWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_pUOLMessengerFacade->GetAccountManager()->AddAccountListEventListener(this);

	// Register as accounts observer...
	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pUOLMessengerFacade->GetAccountManager()->GetAccountList(listAccounts);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
		pAccountConnectionManager->RegisterAccountObserver(pAccount, this);
	}

	LoadDefaultIcon();
	AddIcon();

	LoadDefaultTooltip();

	return 0L;
}

void CUOLMessengerTrayWindow::OnDestroy()
{
	RemoveIcon();

	// Unregister as account observer...
	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	m_pUOLMessengerFacade->GetAccountManager()->GetAccountList(listAccounts);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	for (POSITION pos = listAccounts.GetHeadPosition(); pos; )
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);
		pAccountConnectionManager->UnregisterAccountObserver(pAccount, this);
	}
}

void CUOLMessengerTrayWindow::OnLButtonDblClick(UINT nFlags, CPoint& ptClick)
{
	NotifyTrayWindowEventListeners(&CUOLMessengerTrayWindowEventListener::OnLButtonDblClick, ptClick);
}

void CUOLMessengerTrayWindow::OnLButtonDown(UINT nFlags, CPoint& ptClick)
{
	NotifyTrayWindowEventListeners(&CUOLMessengerTrayWindowEventListener::OnLButtonDown, ptClick);
}

void CUOLMessengerTrayWindow::OnLButtonUp(UINT nFlags, CPoint& ptClick)
{
	NotifyTrayWindowEventListeners(&CUOLMessengerTrayWindowEventListener::OnLButtonUp, ptClick);
}

void CUOLMessengerTrayWindow::OnRButtonDown(UINT nFlags, CPoint& ptClick)
{
	NotifyTrayWindowEventListeners(&CUOLMessengerTrayWindowEventListener::OnRButtonDown, ptClick);
}

void CUOLMessengerTrayWindow::OnRButtonUp(UINT nFlags, CPoint& ptClick)
{
	NotifyTrayWindowEventListeners(&CUOLMessengerTrayWindowEventListener::OnRButtonUp, ptClick);
}

void CUOLMessengerTrayWindow::LoadDefaultIcon()
{
	m_iconCurrent.Detach();
	m_iconCurrent = GetStatusIcon(IUOLMessengerAccountConnectionManager::AS_OFFLINE);

	SetIcon(m_iconCurrent);
}

void CUOLMessengerTrayWindow::LoadDefaultTooltip()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strToolTip;
	strToolTip = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

	SetTip(strToolTip);
}

CIconHandle CUOLMessengerTrayWindow::GetStatusIcon(IUOLMessengerAccountConnectionManager::AccountStatus status)
{
	CIconHandle	icon;

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager)
	{
		CSystemTrayIconListElementPtr pSystemTrayIconList = ap_dynamic_cast<CSystemTrayIconListElementPtr>(pUIManager->GetWindowManager()->GetElement("__SystemTrayIconListInstance"));

		if (pSystemTrayIconList)
		{
			CAbstractStatusTrayIconElementPtr pAbstractStatusTrayIconElement = pSystemTrayIconList->GetStatusTrayIcon(status);

			if (pAbstractStatusTrayIconElement)
			{
				CIconElementPtr pIconElement = pAbstractStatusTrayIconElement->GetIcon();

				if (pIconElement)
				{
					icon = pIconElement->GetIcon();
				}
			}
		}
	}

	return icon;
}