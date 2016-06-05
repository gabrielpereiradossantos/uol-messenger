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
 
// AccountsPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "AccountsPropPage.h"

#include "../../Controller/UIMApplication.h"
#include "../image/imagebuilder.h"
#include "MinDependencySpecialFolder.h"

#include "AccountEditDialog.h"
#include ".\accountsproppage.h"

// CAccountsPropPage dialog
CAccountsPropPage::CAccountsPropPage() : 
	CPreferencesPropPage(IDD_DIALOG_ACCOUNTS)
{

	m_strConnectCaption.LoadString(IDS_ACCOUNTS_PROPPAGE_BUTTON_CONNECT);
	m_strDisconnectCaption.LoadString(IDS_ACCOUNTS_PROPPAGE_BUTTON_DISCONNECT);
	m_strNewUserId.LoadString(IDS_ACCOUNTS_PROPPAGE_NEW_USER_ID);
}

CAccountsPropPage::~CAccountsPropPage()
{
}


void CAccountsPropPage::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	if (m_pCurrentAccount.get() == pAccount.get())
	{
		DWORD dwConnectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(pAccount);

		ShowAccountConnectionState(dwConnectionState);
	}
}

void CAccountsPropPage::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (m_pCurrentAccount.get() == pAccount.get())
	{
		DWORD dwConnectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(pAccount);

		ShowAccountConnectionState(dwConnectionState);
	}
}

void CAccountsPropPage::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	OnDisconnected(pAccount);
}

void CAccountsPropPage::OnStatusChanged(IUOLMessengerAccountPtr pAccount) 
{ 
	if(m_wndListAccounts2.IsWindowVisible() )
	{
		m_wndListAccounts2.RedrawWindow();
	}
}

//
// CAccountsPropPage message handlers
//
LRESULT CAccountsPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	m_wndButtonConnect = GetDlgItem(IDC_BUTTON_CONNECT);
	
	InitAccountList();

	OnSelChangeAccountListImpl();

	_Module.GetMessageLoop()->AddIdleHandler(this);

	return 0L;
}


void CAccountsPropPage::OnDestroy()
{
	_Module.GetMessageLoop()->RemoveIdleHandler(this);

	ReleaseAccounts();
}

void CAccountsPropPage::EditAccount()
{
	int nCursel;
	AccountHelper* pAcountHelper = GetCurrentSelectionAccountHelper(nCursel);
	if (pAcountHelper == NULL)
	{
		return;
	}

	CAccountEditDialog dialog(pAcountHelper->pAccount, OM_CHANGE_ACCOUNT);
	if (dialog.DoModal() == IDOK)
	{
		int nCurSel;
		AccountHelper* pAh  = GetCurrentSelectionAccountHelper(nCurSel);

		m_wndListAccounts2.DeleteString(nCurSel);

		int nNewItem = m_wndListAccounts2.AddString(FormatAccountUserId(pAh->pAccount));
		m_wndListAccounts2.SetItemData(nNewItem, (DWORD_PTR) pAh);
		m_wndListAccounts2.SetCurSel(nNewItem);

		OnSelChangeAccountListImpl();
	}
}

void CAccountsPropPage::OnButtonChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	EditAccount();
}

void CAccountsPropPage::OnButtonAddAccount(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	OnButtonAddAccountImpl(NULL);
}


void CAccountsPropPage::OnButtonRemove(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	int nCurSelItem = m_wndListAccounts2.GetCurSel();
	if (nCurSelItem == -1)
	{
		return;
	}

	// Format confirmation message...
	CString strMsgFormat;
	strMsgFormat.LoadString(IDS_ACCOUNTS_PROPPAGE_REMOVE_ACCOUNT_MSG_FORMAT);

	CString strSelText = "";
	AccountHelper* pAh;
	IUOLMessengerAccountPtr pAccount;
	DWORD_PTR pData = m_wndListAccounts2.GetItemData(nCurSelItem);
	if (pData != NULL)
	{
		// Get selection info
		pAh = (AccountHelper*) pData;
		if (pAh != NULL)
		{
			pAccount = pAh->pAccount;
			strSelText.Format(_T("%s"), pAh->pAccount->GetUserId());
		}
	}
	
	CString strMessage;
	strMessage.Format(strMsgFormat, strSelText);

	// Confirm removal
    UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION);
	if (IDYES == ret)
	{
		DWORD_PTR pData = m_wndListAccounts2.GetItemData(nCurSelItem);
		if (pData != NULL)
		{
			// Get selection info
			AccountHelper* pAh = (AccountHelper*) pData;
			IUOLMessengerAccountPtr pAccount = pAh->pAccount;
			
			if (pAccount != NULL)
			{
				// Remove/release account...
				CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(pAccount, this);

				CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(pAccount);

				m_wndListAccounts2.DeleteString(nCurSelItem);
				delete pAh;

				// Update UI...
				if (m_wndListAccounts2.GetCount() > 0)
				{
					m_wndListAccounts2.SetCurSel(0);
					OnSelChangeAccountListImpl();
				}
				else
				{
					// No more items, disable button
					m_pCurrentAccount = IUOLMessengerAccountPtr();
					GetDlgItem(IDC_BUTTON_REMOVE).EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_CONNECT).EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_CHANGE).EnableWindow(FALSE);
				}
			}
		}
	}
}


void CAccountsPropPage::OnButtonConnect(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	int nCurSelItem = m_wndListAccounts2.GetCurSel();
	if (nCurSelItem != -1)
	{
		AccountHelper* pAh = GetAccountHelper(nCurSelItem);

		if (pAh != NULL)
		{
			IUOLMessengerAccountPtr pAccount = pAh->pAccount;
	
			CString strButtonText;
			m_wndButtonConnect.GetWindowText(strButtonText);

			// Connect/disconnect account
			if (0 == strButtonText.Compare(m_strConnectCaption))
			{
				if (CUIMApplication::GetApplication()->GetAccountConnectionManager()->Connect(pAccount))
				{
					ShowAccountConnectionState(IUOLMessengerAccountConnectionManager::CS_CONNECTING);
				}
			}
			else if (0 == strButtonText.Compare(m_strDisconnectCaption))
			{
				CUIMApplication::GetApplication()->GetAccountConnectionManager()->Disconnect(pAccount);
			}
		}
	}
}


BOOL CAccountsPropPage::OnIdle()
{
	BOOL bEnableRemove = FALSE;

	if (m_pCurrentAccount != NULL)
	{
		DWORD dwConnectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(m_pCurrentAccount);
		bEnableRemove = (IUOLMessengerAccountConnectionManager::CS_DISCONNECTED == dwConnectionState);
	}

	CWindow wndRemove = GetDlgItem(IDC_BUTTON_REMOVE);
	if (wndRemove.IsWindow())
	{
		wndRemove.EnableWindow(bEnableRemove);
	}

	return TRUE;
}


void CAccountsPropPage::OnButtonAddAccountImpl(IUOLMessengerPropertyPageCommand* pPropPageCommand)
{
	IUOLMessengerAccountPtr pAccount;
	CString strAccountType;
	CString strUserName;
	CString strPassword;
	BOOL bSavePassword;
	
	if (pPropPageCommand)
	{
		if (pPropPageCommand->GetPropPageCommandType() == 
				IUOLMessengerPropertyPageCommandFactory::PP_COMMAND_TYPE_NEW_ACCOUNT)
		{
			// This casting is only necessary, because this command has a 
			// calculated field.
			// Usually we will get the values hold by the command using 
			// "GetCommandValueXXX".
			//
			CNewAccountPropertyPageCommand* pNewAccountCmd = 
					dynamic_cast<CNewAccountPropertyPageCommand*>(pPropPageCommand);
			
			if (pNewAccountCmd)
			{
				BOOL resultOk = FALSE;
				
				resultOk = pNewAccountCmd->GetAccountType(strAccountType);
				
				// Testing code (Not used in fact).
				
				if (resultOk)
				{
					resultOk = pNewAccountCmd->GetCommandValueString(
							IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_USERNAME, strUserName);
				}
				
				if (resultOk)
				{
					resultOk = pNewAccountCmd->GetCommandValueString(
						IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_PASSWORD, strPassword);
				}
				
				if (resultOk)
				{
					resultOk = pNewAccountCmd->GetCommandValueBOOL(
						IUOLMessengerPropertyPageCommand::CVT_ACCOUNT_SAVE_PASSWORD_FLAG, bSavePassword);
				}
				
				// Testing code (Not used in fact).
			}
		}
	}
		
	if (!strAccountType.IsEmpty())
	{
		pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(_T(""), _T(strAccountType));
	}
	else
	{
		pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(_T(""), _T("prpl-msn"));
	}

	CAccountEditDialog dialog(pAccount, OM_ADD_ACCOUNT);
	
	if (IDOK == dialog.DoModal())
	{
		CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(pAccount);

		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(pAccount, this);
		
		AccountHelper* pAh = new AccountHelper;
		pAh->pAccount = pAccount;
		
		int nNewItem = m_wndListAccounts2.AddString(FormatAccountUserId(pAccount));
		m_wndListAccounts2.SetItemData(nNewItem, (DWORD_PTR) pAh);
		m_wndListAccounts2.SetCurSel(nNewItem);

		OnSelChangeAccountListImpl();
	}
	else
	{
		CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(pAccount);
	}
}


LRESULT CAccountsPropPage::OnLbnSelchangeListAccounts2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OnSelChangeAccountListImpl();

	return 0;
}

LRESULT CAccountsPropPage::OnItemChangedAccountList(NMHDR *pNMHDR)
{
	OnSelChangeAccountListImpl();

	return 0L;
}


int CALLBACK CAccountsPropPage::CompareAccountHelper(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	AccountHelper* pAh1 = (AccountHelper*) lParam1;
	AccountHelper* pAh2 = (AccountHelper*) lParam2;

	ATLASSERT((pAh1->pAccount != NULL) && (pAh2->pAccount != NULL));

	if ((pAh1->pAccount != NULL) && (pAh2->pAccount != NULL))
	{
		return ((pAh1->pAccount)->GetUserId().Compare((pAh2->pAccount)->GetUserId()));
	}

	return 0;
}


void CAccountsPropPage::InitAccountList()
{
	m_wndListAccounts2.SubclassWindow(GetDlgItem(IDC_LIST_ACCOUNTS2));

	CAtlList<IUOLMessengerAccountPtr> listAccounts;
	CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(listAccounts);

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	// Populate account list
	int nItemCount = 0;
	for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
	{
		IUOLMessengerAccountPtr pAccount = listAccounts.GetNext(pos);

		if (pAccount != NULL)
		{
			AccountHelper* pAh = new AccountHelper;
			pAh->pAccount = pAccount;

			int nItem = m_wndListAccounts2.AddString(FormatAccountUserId(pAccount));
			m_wndListAccounts2.SetItemData(nItem, (DWORD_PTR) pAh);

			pAccountConnectionManager->RegisterAccountObserver(pAccount, this);

			nItemCount++;
		}
	}
}


CString CAccountsPropPage::FormatAccountUserId(IUOLMessengerAccountPtr pAccount)
{
	CString strAccountId;
	strAccountId.Format(_T("%s"), pAccount->GetUserId());

	return strAccountId;
}


CAccountsPropPage::AccountHelper* CAccountsPropPage::GetAccountHelper(int nInd)
{
    DWORD_PTR pData = m_wndListAccounts2.GetItemData(nInd);
	if (pData != NULL)
	{
		return (AccountHelper*) pData;
	}

	return NULL;
}


CAccountsPropPage::AccountHelper* CAccountsPropPage::GetCurrentSelectionAccountHelper(int& nCurSel)
{
	int nCurSelItem = m_wndListAccounts2.GetCurSel();
	if (-1 != nCurSelItem)
	{
		nCurSel = nCurSelItem;
		return GetAccountHelper(nCurSelItem);
	}

	return NULL;
}

void CAccountsPropPage::ReleaseAccounts()
{
	if (m_wndListAccounts2.IsWindow())
	{
		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

		int nAccountCount = m_wndListAccounts2.GetCount();
		AccountHelper* pAh;
		for (int nInd = 0; nInd < nAccountCount; nInd++)
		{
			pAh = GetAccountHelper(nInd);
			if (pAh != NULL)	
			{
				pAccountConnectionManager->UnregisterAccountObserver(pAh->pAccount, this);

				delete pAh;
			}
		}

		int iLoopProt = 0;
		while(m_wndListAccounts2.GetCount() > 0 && ++iLoopProt < 100000)
		{
			m_wndListAccounts2.DeleteString(0);
		}

	}
}


void CAccountsPropPage::OnSelChangeAccountListImpl()
{
	BOOL bEnableButtons = FALSE;

	IUOLMessengerAccountPtr pAccount;

	int nCurSelItem = m_wndListAccounts2.GetCurSel();

	if (CB_ERR != nCurSelItem)
	{
		// Get account....
		DWORD_PTR pData = m_wndListAccounts2.GetItemData(nCurSelItem);

		if (pData != LB_ERR)
		{
			ATLASSERT(pData != NULL);

			if (pData != NULL)
			{
				AccountHelper* pAh = (AccountHelper*) pData;
				pAccount = pAh->pAccount;

				// Update account data...
				if (pAccount != NULL)
				{
					DWORD dwConnectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(pAccount);

					ShowAccountConnectionState(dwConnectionState);
				}
				bEnableButtons = TRUE;
			}
		}
	} 

	m_pCurrentAccount = pAccount;

	m_wndButtonConnect.EnableWindow(bEnableButtons);
	GetDlgItem(IDC_BUTTON_CHANGE).EnableWindow(bEnableButtons);
}


void CAccountsPropPage::ShowAccountConnectionState(DWORD dwConnectionState)
{
	// Change connect/disconnect button caption and enable/disable controls...
	if (dwConnectionState == IUOLMessengerAccountConnectionManager::CS_DISCONNECTED)
	{
		m_wndButtonConnect.SetWindowText(m_strConnectCaption);
	}
	else
	{
		m_wndButtonConnect.SetWindowText(m_strDisconnectCaption);
	}
}


BOOL CAccountsPropPage::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}

LRESULT CAccountsPropPage::OnLbnDblclkListAccounts2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EditAccount();
	return 0;
}

LRESULT CAccountsPropPage::OnNMDblclkListAccounts(int idCtrl, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	EditAccount();
	return 0;
}

LRESULT CAccountsPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	//::SetBkColor(hdc, )
	return (LRESULT) m_brushStatic.m_hBrush;
}

LRESULT CAccountsPropPage::OnNewAccount(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL resultOk = FALSE;
	
	if (lParam)
	{
		IUOLMessengerPropertyPageCommand* pPropPageCommand = 
				reinterpret_cast<IUOLMessengerPropertyPageCommand*>(lParam);
		
		OnButtonAddAccountImpl(pPropPageCommand);
		
		resultOk = TRUE;
	}
	
	return resultOk;
}

