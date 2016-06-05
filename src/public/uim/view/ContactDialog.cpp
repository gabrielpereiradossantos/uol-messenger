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
#include "ContactDialog.h"

#include "../controller/UIMApplication.h"
#include "../resource.h"


CContactDialog::CContactDialog() :
		CMainDialog(IDD_DIALOG_ADD_CONTACT)
{
}


CContactDialog::~CContactDialog(void)
{
}


void CContactDialog::Initialize(const CString& strContact, IUOLMessengerAccountPtr pAccount)
{
	m_strContact = strContact;
	m_pAccount = pAccount;
}


void CContactDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
    __super::SetParameters(pMapParams, pElementTable);
}


LRESULT CContactDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	__super::OnInitDialog(NULL, NULL);

	DoDataExchange(FALSE);

	UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	if (pStringLoader)
	{
		CString strTitleFormat;
		CString strTitleParam;
		
		strTitleFormat = pStringLoader->GetValue(_T("uol.resource.IDS_TITLE_WINDOW_DEFAULT"));
		strTitleParam = pStringLoader->GetValue(_T("uol.resource.IDS_ADD_CONTACT_DIALOG_TITLE"));

		ATLASSERT(FALSE == strTitleFormat.IsEmpty());
		ATLASSERT(FALSE == strTitleParam.IsEmpty());

		CString strTitle;
		strTitle.Format(strTitleFormat, strTitleParam);

		//Set title and text
		SetTitle(strTitle);
	}

	CEdit wndEditContact = GetDlgItem(IDC_EDIT_USER);
	ATLASSERT(wndEditContact.IsWindow());
	wndEditContact.LimitText(IUOLMessengerContact::MAX_CONTACT_NAME_LENGTH);

	ATLASSERT(m_wndComboGroups.IsWindow());
	m_wndComboGroups.LimitText(IUOLMessengerGroup::MAX_GROUP_NAME_LENGTH);

	m_wndComboAccounts.SubclassWindow(GetDlgItem(IDC_COMBO_ACCOUNT));

	FillGroupList();
	FillAccountList();

	if (m_pAccount != NULL)
	{
		CString strUserId;
		strUserId.Format(_T("%s"), m_pAccount->GetUserId(), m_pAccount->GetProtocolName());

		int nAccountInd = m_wndComboAccounts.FindStringExact(-1, strUserId);

		if (nAccountInd != CB_ERR)
		{
			m_wndComboAccounts.SetCurSel(nAccountInd);
		}
	}
	else if (m_listAccounts.GetCount() > 0)
	{
		m_wndComboAccounts.SetCurSel(0);
	}

	m_userTransparencyApplier.SubclassWindow(GetDlgItem(IDC_ADD_CONTACT_STATIC_USER));
	m_accountTransparencyApplier.SubclassWindow(GetDlgItem(IDC_ADD_CONTACT_STATIC_ACCOUNT));
	m_groupTransparencyApplier.SubclassWindow(GetDlgItem(IDC_ADD_CONTACT_STATIC_GROUP));

	CenterWindow();

	return 0L;
}


LRESULT CContactDialog::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	int nAccount = m_wndComboAccounts.GetCount();
	AccountHelper* pAH;

	for (int i = 0; i < nAccount; i++)
	{
		pAH = (AccountHelper*) m_wndComboAccounts.GetItemData(i);

		if (pAH)
		{
			delete pAH;
		}
	}

	return 0L;
}


void CContactDialog::OnBnClickedButtonOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DoDataExchange(TRUE);

	if (AddContact())
	{
		EndDialog(IDOK);
	}
}


void CContactDialog::OnBnClickedButtonCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	EndDialog(IDCANCEL);
}


void CContactDialog::FillGroupList()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	CUIMApplication::GetApplication()->GetContactList()->GetGroupList(m_listGroups);

	for (POSITION pos = m_listGroups.GetHeadPosition(); pos;)
	{
		IUOLMessengerGroupPtr pGroup = m_listGroups.GetNext(pos);
		CString strGroupName = pGroup->GetName();

		COMBOBOXEXITEM cbei = {0};
		cbei.iItem = -1;
		cbei.mask = CBEIF_TEXT;
		cbei.pszText = (LPTSTR) (LPCTSTR) strGroupName;
		cbei.cchTextMax = strGroupName.GetLength();

		//m_wndComboGroups.InsertItem(&cbei);
		m_wndComboGroups.AddString((LPCTSTR) strGroupName);
	}
	
	if (m_listGroups.GetCount() > 0)
	{
		m_wndComboGroups.SetCurSel(0);
	}
}


void CContactDialog::FillAccountList()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	if (!m_pAccount)
	{
		CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(m_listAccounts);
	}
	else
	{
		m_listAccounts.AddTail(m_pAccount);
	}

	AccountHelper* pAH;
	IUOLMessengerAccountPtr pAccount;
	int nIndex;
	BOOL bAddAccount;

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	for (POSITION pos = m_listAccounts.GetHeadPosition(); pos;)
	{
		pAccount = m_listAccounts.GetAt(pos);
		bAddAccount = FALSE;

		if ((pAccount) && (pAccountConnectionManager->IsConnected(pAccount)))
		{
			CIMProtocol* pProtocol = pAccount->GetProtocol();
			ATLASSERT(pProtocol);

			if (pProtocol)
			{
				CIMProtocol3* pProtocol3 = dynamic_cast<CIMProtocol3*>(pProtocol);

				if (pProtocol3)
				{
					bAddAccount = pProtocol3->HasContactListSupport();
				}
			}
		}

		if (bAddAccount)
		{
			pAH = new AccountHelper;
			pAH->pAccount = pAccount;

			CString strUser;
			strUser.Format(_T("%s"), pAH->pAccount->GetUserId());

			nIndex = m_wndComboAccounts.AddString(strUser);

			m_wndComboAccounts.SetItemData(nIndex, (DWORD_PTR)pAH);

			m_listAccounts.GetNext(pos);
		}
		else
		{
			POSITION posRemoved = pos;

			m_listAccounts.GetNext(pos);
			m_listAccounts.RemoveAt(posRemoved);
		}
	}
}


BOOL CContactDialog::AddContact()
{
	BOOL bAdded = FALSE;

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CString strMsgText;

	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	m_strContact.Trim();

	// Test empty contact...
	if (m_strContact.IsEmpty())
	{
		DoDataExchange(FALSE, IDC_EDIT_USER);
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_NAME_NOTFOUND");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bAdded;
	}

	CString strGroup;
	m_wndComboGroups.GetWindowText(strGroup);

	strGroup.Trim();

	// Test empty group...
	if (strGroup.IsEmpty())
	{
		m_wndComboGroups.SetWindowText(strGroup);
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_GROUP_NOTFOUND");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bAdded;
	}

	// Test selected account...
	if (m_wndComboAccounts.GetCurSel() < 0)
	{
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_ACCOUNT_NOTFOUND");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bAdded;
	}

	AccountHelper* pAH = (AccountHelper*)m_wndComboAccounts.GetItemData(m_wndComboAccounts.GetCurSel());
		
	IUOLMessengerAccountPtr pAccount = pAH->pAccount;

	if (pAccount)
	{
		BOOL bShowGenericError = FALSE;

		CIMProtocol* pProtocol = pAccount->GetProtocol();

		if (pProtocol != NULL)
		{
			if (pProtocol->IsValidUserName((LPCTSTR) m_strContact))
			{
				IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount, m_strContact);

				if (pContact != NULL)
				{
					CString strDefaultGroup = pProtocol->GetDefaultContactGroup().c_str();

					if ((pProtocol->AutoAddContactOnAuthorization()) &&
						(FALSE == strDefaultGroup.IsEmpty()) &&
						(0 == strDefaultGroup.CompareNoCase(pContact->GetGroup()->GetName())) &&
						(0 != strDefaultGroup.CompareNoCase(strGroup)))
					{
						// Trying to add existent contact (in default group): 
						// move from default group to selected group

						IUOLMessengerGroupPtr pGroup = CUIMApplication::GetApplication()->GetContactList()->AddGroup(strGroup);
						ATLASSERT(pGroup);

						bAdded = CUIMApplication::GetApplication()->GetRemoteContactList()->MoveContact(pAccount, m_strContact, strDefaultGroup, strGroup);
						bShowGenericError = !bAdded;
					}
					else
					{
						CString strMsgFormat;
						strMsgFormat = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_CONTACT_ALREADY_EXISTS_MSG_FORMAT");
						strMsgText.Format(strMsgFormat, m_strContact, pContact->GetGroup()->GetName());

						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
					}
				}
				else 
				{
					CIMProtocol* pProtocol = pAccount->GetProtocol();

					if ((CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(pAccount, m_strContact)) && 
						(pProtocol != NULL) &&
						(FALSE == (pProtocol->GetPrivacyFlags() & PRIVACY_FLAGS_HAS_PERMIT_LIST)))
					{
						CString strFormat;
						strFormat = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_BLOCK_LIST");
						strMsgText.Format(strFormat, m_strContact);
							
						CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
					}
					else
					{
						bAdded = CUIMApplication::GetApplication()->GetRemoteContactList()->AddContact(pAccount, m_strContact, strGroup);
						bShowGenericError = !bAdded;
					}
				}
			}
			else
			{
				CString strMsgFormat;
				strMsgFormat = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_INVALID_USERNAME_FORMAT");
				strMsgText.Format(strMsgFormat, m_strContact, pProtocol->GetName().c_str());

				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
			}
		}

		if (bShowGenericError)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Could not retrieve the protocol for user [%s].", 
					pAccount->GetUserId());

			strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_ERROR_ADD_CONTACT");
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		}
	}

	return bAdded;
}
