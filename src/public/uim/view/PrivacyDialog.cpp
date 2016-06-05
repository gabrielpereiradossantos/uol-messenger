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
#include "PrivacyDialog.h"

#include <commands/launchinfowindowcommand.h>
#include <commands/BlockContactCommand.h>
#include <commands/UnblockContactCommand.h>

CPrivacyDialog::CPrivacyDialog() :
		CMainDialog(IDD_DIALOG_PRIVACY)
{
}

CPrivacyDialog::~CPrivacyDialog()
{
}

LRESULT CPrivacyDialog::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	CString strTitle;
	GetWindowText(strTitle);

	__super::OnInitDialog(NULL, NULL);

	CenterWindow();

	//Set title and text
	SetTitle(strTitle);

	m_wndListPermited.SubclassWindow(GetDlgItem(IDC_LIST_PERMITED));
	m_wndListDenied.SubclassWindow(GetDlgItem(IDC_LIST_DENIED));
	m_wndComboAccounts.SubclassWindow(GetDlgItem(IDC_COMBO_ACCOUNT));

	m_wndButtonPermit = GetDlgItem(IDC_BUTTON_PERMIT);
	m_wndButtonDeny = GetDlgItem(IDC_BUTTON_DENY);
	m_wndButtonInfo = GetDlgItem(IDC_BUTTON_INFO);

	FillAccountList();

	CUIMApplication::GetApplication()->GetPrivacyManager()->RegisterObserver(this);

	if (m_wndComboAccounts.GetCount() > 0)
	{
		m_wndComboAccounts.SetCurSel(0);

		OnSelChangeComboAccountImpl();
	}

	m_labelAccountTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_PRIVACY_ACCOUNT));
	m_labelAllowTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_PRIVACY_ALLOW));
	m_labelBlockTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_PRIVACY_BLOCK));

	return 0L;
}

void CPrivacyDialog::OnClose()
{
	EndDialog(0);
}

void CPrivacyDialog::OnDestroy()
{
	CUIMApplication::GetApplication()->GetPrivacyManager()->UnregisterObserver(this);

	ClearPermissionList(m_wndListPermited);
	ClearPermissionList(m_wndListDenied);
	ClearAccountList();
}


void CPrivacyDialog::OnSelChangeComboAccount(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	OnSelChangeComboAccountImpl();
}

void CPrivacyDialog::OnSelChangeListBoxPermited(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	m_wndButtonPermit.EnableWindow(FALSE);
	m_wndButtonDeny.EnableWindow(TRUE);
	m_wndButtonInfo.EnableWindow(TRUE);

	m_wndListDenied.SetCurSel(-1);
}


void CPrivacyDialog::OnSelChangeListBoxDenied(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	m_wndButtonPermit.EnableWindow(TRUE);
	m_wndButtonDeny.EnableWindow(FALSE);
	m_wndButtonInfo.EnableWindow(TRUE);

	m_wndListPermited.SetCurSel(-1);
}


void CPrivacyDialog::OnClickedButtonPermit(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	TogglePermission(m_wndListDenied, PF_PERMITED);
}


void CPrivacyDialog::OnClickedButtonDeny(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	TogglePermission(m_wndListPermited, PF_DENIED);
}


void CPrivacyDialog::OnClickedButtonInfo(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DWORD_PTR pItemData;
	ContactData* pCD;
	int nIndex;

	nIndex = m_wndListDenied.GetCurSel();

	if (nIndex == -1)
	{
		pItemData = (DWORD_PTR) m_wndListPermited.GetItemData(m_wndListPermited.GetCurSel());
	}
	else
	{
		pItemData = (DWORD_PTR) m_wndListDenied.GetItemData(nIndex);
	}

	if (pItemData != LB_ERR)
	{
		pCD = (ContactData*) pItemData;

		CUIMApplication::GetApplication()->ExecuteCommand(new CLaunchInfoWindowCommand(pCD->pContact, m_hWnd));
	}
}


void CPrivacyDialog::OnPermitAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	if (pAccount.get() == m_pCurrentAccount.get())
	{
		UpdatePermission(strContact, m_wndListDenied, m_wndListPermited, m_mapDeniedContacts, m_mapPermitedContacts);
	}
}


void CPrivacyDialog::OnDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	if (pAccount.get() == m_pCurrentAccount.get())
	{
		UpdatePermission(strContact, m_wndListPermited, m_wndListDenied, m_mapPermitedContacts, m_mapDeniedContacts);
	}
}


void CPrivacyDialog::OnPermitRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
}


void CPrivacyDialog::OnDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	if (pAccount.get() == m_pCurrentAccount.get())
	{
		UpdatePermission(strContact, m_wndListDenied, m_wndListPermited, m_mapDeniedContacts, m_mapPermitedContacts);
	}
}


void CPrivacyDialog::FillAccountList()
{
	CAtlList<IUOLMessengerAccountPtr> listAccounts;

	CUIMApplication::GetApplication()->GetAccountManager()->GetAccountList(listAccounts);

	AccountData* pAD;
	IUOLMessengerAccountPtr pAccount;
	CIMProtocol* pProtocol;
	int nIndex;

	IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

	for (POSITION pos = listAccounts.GetHeadPosition(); pos;)
	{
		pAccount = listAccounts.GetNext(pos);

		pProtocol = pAccount->GetProtocol();

		if ((pProtocol != NULL) && 
			(pProtocol->HasContactBlockingSupport()) &&
			(pAccountConnectionManager->IsConnected(pAccount)))
		{
			pAD = new AccountData;
			pAD->pAccount = pAccount;

			CString strAccounText;
			strAccounText.Format(_T("%s"), pAD->pAccount->GetUserId());

			nIndex = m_wndComboAccounts.AddString(strAccounText);

			m_wndComboAccounts.SetItemData(nIndex, (DWORD_PTR) pAD);
		}
	}
}


void CPrivacyDialog::ClearAccountList()
{
	int nCount = m_wndComboAccounts.GetCount();
	AccountData* pAD;

	for (int nInd = 0; nInd < nCount; nInd++)
	{
		pAD = (AccountData*) m_wndComboAccounts.GetItemData(nInd);

		if (pAD)
		{
			delete pAD;
		}
	}
}


void CPrivacyDialog::OnSelChangeComboAccountImpl()
{
	DWORD_PTR pItemData = (DWORD_PTR) m_wndComboAccounts.GetItemData(m_wndComboAccounts.GetCurSel());

	if (pItemData != CB_ERR)
	{
		AccountData* pAD = (AccountData*) pItemData;
	
		// Refill permited and denied contact lists...
		IUOLMessengerAccountPtr pAccount = pAD->pAccount;

		ClearPermissionList(m_wndListDenied);
		ClearPermissionList(m_wndListPermited);

		m_mapDeniedContacts.RemoveAll();
		GetDeniedList(pAccount, m_mapDeniedContacts);
		
		FillPermissionList(m_wndListDenied, m_mapDeniedContacts, CPrivacyDialog::PF_DENIED);

		m_mapPermitedContacts.RemoveAll();
		GetPermitedList(pAccount, m_mapPermitedContacts);
		
		FillPermissionList(m_wndListPermited, m_mapPermitedContacts, CPrivacyDialog::PF_PERMITED);

		m_pCurrentAccount = pAccount;

		m_wndButtonPermit.EnableWindow(FALSE);
		m_wndButtonDeny.EnableWindow(FALSE);
		m_wndButtonInfo.EnableWindow(FALSE);
	}
}


void CPrivacyDialog::GetDeniedList(IUOLMessengerAccountPtr pAccount, CContactMap& mapContacts)
{
	mapContacts.RemoveAll();

	CAtlList<CString> listDeniedContacts;
	CUIMApplication::GetApplication()->GetPrivacyManager()->GetDeniedList(pAccount, listDeniedContacts);

	IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();

	for (POSITION pos = listDeniedContacts.GetHeadPosition(); pos; )
	{
		CString strContact = listDeniedContacts.GetNext(pos);

		IUOLMessengerContactPtr pContact = pContactList->FindContact(pAccount, strContact);

		if (!pContact)
		{
			pContact = pContactList->CreateContact(pAccount, strContact);
		}

		if (pContact)
		{
			mapContacts.SetAt(pContact->GetName(), pContact);
		}
	}
}


void CPrivacyDialog::GetPermitedList(IUOLMessengerAccountPtr pAccount, CContactMap& mapContacts)
{
	mapContacts.RemoveAll();

	IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();

	CIMProtocol* pProtocol = pAccount->GetProtocol();

	if (pProtocol)
	{
		int nPrivacyFlags = pProtocol->GetPrivacyFlags();

		if ((nPrivacyFlags & PRIVACY_FLAGS_NO_LIST_INTERSECTION) &&
			(nPrivacyFlags & PRIVACY_FLAGS_HAS_PERMIT_LIST))
		{
			CAtlList<CString> listPermitedContacts;
			CUIMApplication::GetApplication()->GetPrivacyManager()->GetPermitedList(pAccount, listPermitedContacts);

			for (POSITION pos = listPermitedContacts.GetHeadPosition(); pos; )
			{
				CString strContact = listPermitedContacts.GetNext(pos);

				IUOLMessengerContactPtr pContact = pContactList->FindContact(pAccount, strContact);

				if (!pContact)
				{
					pContact = pContactList->CreateContact(pAccount, strContact);
				}

				if (pContact)
				{
					mapContacts.SetAt(pContact->GetName(), pContact);
				}
			}
		}
	}

	// Extract permited contacts from contact list
	CAtlList<IUOLMessengerContactPtr> listAccountContacts;
	if (pContactList->FindContacts(pAccount, listAccountContacts))
	{
		CContactMap::CPair* pPair;
		for (POSITION pos = listAccountContacts.GetHeadPosition(); pos; )
		{
			IUOLMessengerContactPtr pContact = listAccountContacts.GetNext(pos);
			
			pPair = mapContacts.Lookup(pContact->GetName());
			if ((!pPair) && 
				(FALSE == CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(pAccount, pContact->GetName())))
			{
				mapContacts.SetAt(pContact->GetName(), pContact);
			}
		}
	}
}


void CPrivacyDialog::FillPermissionList(CListBox wndList, CContactMap& mapContacts, int nPermissionFlags)
{
	// Fill permission list box with original contact privacy permission flags
	ATLASSERT(wndList.IsWindow());

	ContactData* pCD;

	int nIndex;
	for (POSITION pos = mapContacts.GetStartPosition(); pos; )
	{
		IUOLMessengerContactPtr pContact = mapContacts.GetNext(pos)->m_value;
		nIndex = wndList.AddString(pContact->GetAlias());

		if (nIndex >= 0)
		{
			pCD = new ContactData;
			pCD->pContact = pContact;
			pCD->nPermissionFlags = nPermissionFlags;

			wndList.SetItemDataPtr(nIndex, pCD);
		}
	}
}


void CPrivacyDialog::ClearPermissionList(CListBox wndList)
{
	int nCount = wndList.GetCount();
	DWORD_PTR pItemData;
	ContactData* pCD;

	for (int nInd = 0; nInd < nCount; nInd++)
	{
		pItemData = (DWORD_PTR) wndList.GetItemDataPtr(nInd);

		if (LB_ERR != pItemData)
		{
			pCD = (ContactData*) pItemData;

			delete pCD;
		}
	}

	wndList.ResetContent();
}


void CPrivacyDialog::TogglePermission(CListBox wndFromList, int nPermissionFlags)
{
	int nCurSel = wndFromList.GetCurSel();

	m_wndButtonPermit.EnableWindow(FALSE);
	m_wndButtonDeny.EnableWindow(FALSE);

	DWORD_PTR pItemData = (DWORD_PTR) wndFromList.GetItemDataPtr(nCurSel);

	if (LB_ERR != pItemData)
	{
		ContactData* pCD = (ContactData*) pItemData;

		IUOLMessengerCommandPtr pCommand;

		if (nPermissionFlags == PF_PERMITED)
		{
			pCommand = new CUnblockContactCommand(pCD->pContact, m_hWnd);
			pCommand->Execute();
		}
		else if (nPermissionFlags == PF_DENIED)
		{
			pCommand = new CBlockContactCommand(pCD->pContact, m_hWnd);
			pCommand->Execute();
		}
		//OnSelChangeComboAccountImpl();
	}
}


void CPrivacyDialog::UpdatePermission(const CString& strContact, CListBox wndFromList, CListBox wndToList, CContactMap& m_mapFrom, CContactMap& m_mapTo)
{
	CContactMap::CPair* pPair;

	::SendMessage(wndToList.m_hWnd, CUOLContactList::WM_SET_IS_UPDATING, TRUE, 0);
	::SendMessage(wndFromList.m_hWnd, CUOLContactList::WM_SET_IS_UPDATING, TRUE, 0);

	pPair = m_mapFrom.Lookup(strContact);

	if (pPair)
	{
		IUOLMessengerContactPtr pContact = pPair->m_value;

		m_mapTo.SetAt(strContact, pContact);
		m_mapFrom.RemoveKey(strContact);

		CString strAlias = pContact->GetAlias();

		//int nFromIndex = wndFromList.FindString(-1, (LPCTSTR) strAlias);
		//int nFromIndex = wndFromList.FindString(-1, (LPCTSTR) strAlias);
		int nFromIndex = -1;
		for (int iCont = 0; iCont < wndFromList.GetCount(); ++iCont)
		{
			ContactData* pCD = (ContactData*) wndFromList.GetItemData(iCont);
			if(pCD->pContact->GetAlias() == strAlias)
			{
				nFromIndex = iCont;
				break;
			}
		}

		if (nFromIndex >= 0)
		{
			DWORD_PTR pItemData = (DWORD_PTR) wndFromList.GetItemDataPtr(nFromIndex);

			if (LB_ERR != pItemData)
			{
				ContactData* pCD = (ContactData*) pItemData;

				int nIndex = wndToList.AddString(strAlias);
				ATLASSERT(nIndex >= 0);

				if (nIndex >= 0)
				{
					wndToList.SetItemDataPtr(nIndex, pCD);
					wndFromList.DeleteString(nFromIndex);

					int nCount = wndFromList.GetCount();

					if (nFromIndex < nCount)
					{
						wndFromList.SetCurSel(nFromIndex);

						if (wndFromList == m_wndListPermited)
						{
							m_wndButtonDeny.EnableWindow(TRUE);
						}
						else if (wndFromList == m_wndListDenied)
						{
							m_wndButtonPermit.EnableWindow(TRUE);
						}
					}
					else if (nCount > 0)
					{
						wndFromList.SetCurSel(nCount - 1);

						if (wndFromList == m_wndListPermited)
						{
							m_wndButtonDeny.EnableWindow(TRUE);
						}
						else if (wndFromList == m_wndListDenied)
						{
							m_wndButtonPermit.EnableWindow(TRUE);
						}
					}
					else
					{
						m_wndButtonPermit.EnableWindow(FALSE);
						m_wndButtonDeny.EnableWindow(FALSE);
					}
				}
			}
		}
	}

	::SendMessage(wndToList.m_hWnd, CUOLContactList::WM_SET_IS_UPDATING, FALSE, 0);
	::SendMessage(wndFromList.m_hWnd, CUOLContactList::WM_SET_IS_UPDATING, FALSE, 0);

	wndToList.Invalidate(FALSE);
	wndToList.UpdateWindow();
	wndFromList.Invalidate(FALSE);
	wndFromList.UpdateWindow();
}
