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
#include "AuthorizeDialog.h"

#include "../controller/UIMApplication.h"
#include <commands/ShowContactDialogCommand.h>
#include "DialogManager.h"


CAuthorizeDialog::CAuthorizeDialog(void) :
		CMainDialog(IDD_DIALOG_AUTHORIZE)
{
}


CAuthorizeDialog::~CAuthorizeDialog(void)
{
}


void CAuthorizeDialog::Initialize(IUOLMessengerAccountPtr pAccount, const CString& strContact, BOOL bAlreadyAdded)
{
	m_pAccount = pAccount;
	m_strContact = strContact;
	m_bAlreadyAdded = bAlreadyAdded;
	m_bBlockContact = FALSE;
	m_bAddContact = !bAlreadyAdded;
}


BOOL CAuthorizeDialog::IsContactAuthorized() const
{
	return (FALSE == m_bBlockContact);
}


BOOL CAuthorizeDialog::GetAddContactFlag() const
{
	return m_bAddContact;
}


CString CAuthorizeDialog::GetContactGroup() const
{
	return m_strAddGroup;
}


LRESULT CAuthorizeDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	__super::OnInitDialog(NULL, NULL);

	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	DoDataExchange(FALSE);

	CString strTitle;
	CString strLabel;

	strTitle = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
	strLabel.Format(IDS_YOU_WERE_ADD, m_strContact);

	//Set title and text
	SetTitle(strTitle);
	GetDlgItem(IDC_STATIC_LABEL).SetWindowText(strLabel);

	CWindow wndCheckAddContact = GetDlgItem(IDC_CHECK_ADD_CONTACT);
	CWindow wndGroupComboBox = GetDlgItem(IDC_COMBO_ADD_CONTACT_GROUP);
	BOOL bEnableAddControls = ! m_bAlreadyAdded;

	ATLASSERT(wndCheckAddContact.IsWindow());
	ATLASSERT(wndGroupComboBox.IsWindow());

	wndCheckAddContact.EnableWindow(bEnableAddControls);
	wndGroupComboBox.EnableWindow(bEnableAddControls);

	ATLASSERT(m_wndComboGroups.IsWindow());
	m_wndComboGroups.LimitText(IUOLMessengerGroup::MAX_GROUP_NAME_LENGTH);

	FillGroupList();

	CenterWindow();

    CDialogManager::GetInstance()->RegisterDialog(m_hWnd);

	m_labelTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_LABEL));

	// Register transparent controls
	RegisterControl(GetDlgItem(IDC_CHECK_ADD_CONTACT));
	RegisterControl(GetDlgItem(IDC_RADIO_ALLOW_CONTACT));
	RegisterControl(GetDlgItem(IDC_RADIO_BLOCK_CONTACT));
	SetParentWindow(m_hWnd);
	SetBitmapBackground(m_pActiveBackground);

	return 0L;
}

LRESULT CAuthorizeDialog::OnDestroy()
{
	CDialogManager::GetInstance()->UnregisterDialog(m_hWnd);
    
    return 0L;
}

LRESULT CAuthorizeDialog::OnClose()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	EndDialog(IDCANCEL);
	return 0L;
}


void CAuthorizeDialog::OnBnClickedButtonOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	
	DoDataExchange(TRUE);

	CButton btnCheckAddContact = GetDlgItem(IDC_CHECK_ADD_CONTACT);

	if (FALSE == btnCheckAddContact.IsWindowEnabled())
	{
		ResetAddContactFlag();
	}

	if (GetAddContactFlag())
	{
		CString strMessage;

		m_wndComboGroups.GetWindowText(m_strAddGroup);
		m_strAddGroup.Trim();

		// Check if destination group is empty
		if (m_strAddGroup.IsEmpty())
		{
			m_wndComboGroups.SetWindowText(m_strAddGroup);

			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_GROUP_NOTFOUND");
            CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
			
			return;
		}

		// Check if contact is already included on contact list 
		IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(GetAccount(), m_strContact);

		if (pContact != NULL)
		{
			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			CString strMsgFormat;
			strMsgFormat = pStringLoader->GetValue("uol.resource.error.IDS_CONTACT_DIALOG_CONTACT_ALREADY_EXISTS_MSG_FORMAT");
			strMessage.Format(strMsgFormat, m_strContact, pContact->GetGroup()->GetName());

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

			// Disable add contact controls
			btnCheckAddContact.EnableWindow(FALSE);
			m_wndComboGroups.EnableWindow(FALSE);
			btnCheckAddContact.SetCheck(BST_CHECKED);
			

			return;
		}
	}

	if (m_bBlockContact)
	{
		if (!CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(m_pAccount, m_strContact))
		{
			CUIMApplication::GetApplication()->GetPrivacyManager()->AddDeniedContact(m_pAccount, m_strContact);
		}
	}
	else
	{
		CIMProtocol* pProtocol = m_pAccount->GetProtocol();

		if (pProtocol)
		{
			int nPrivacyFlags = pProtocol->GetPrivacyFlags();

			if (nPrivacyFlags & PRIVACY_FLAGS_NO_LIST_INTERSECTION)
			{
				if (!CUIMApplication::GetApplication()->GetPrivacyManager()->IsPermited(m_pAccount, m_strContact))
				{
					CUIMApplication::GetApplication()->GetPrivacyManager()->AddPermitedContact(m_pAccount, m_strContact);
				}
			}
			else
			{
				if (CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(m_pAccount, m_strContact))
				{
					CUIMApplication::GetApplication()->GetPrivacyManager()->RemoveDeniedContact(m_pAccount, m_strContact);
				}
			}
		}
	}

	EndDialog(IDOK);
}


void CAuthorizeDialog::OnBnClickedButtonCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	EndDialog(IDCANCEL);
}


void CAuthorizeDialog::OnCheckAddContact(UINT /*uCode*/, int /*nID*/, HWND hwndCtrl)
{
	CButton btnAddContact = hwndCtrl;

	m_wndComboGroups.EnableWindow((btnAddContact.GetCheck() == BST_CHECKED));
}


void CAuthorizeDialog::OnBnClickedShowInfo(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->CreateContact(m_pAccount, m_strContact);

	CUIMApplication::GetApplication()->GetUIManager()->LaunchInfoWindow(pContact);
}


IUOLMessengerAccountPtr CAuthorizeDialog::GetAccount()
{
	return m_pAccount;
}


CString CAuthorizeDialog::GetContact() const
{
	return m_strContact;
}


void CAuthorizeDialog::FillGroupList()
{
	CAtlList<IUOLMessengerGroupPtr>	listGroups;

	CUIMApplication::GetApplication()->GetContactList()->GetGroupList(listGroups);

	for (POSITION pos = listGroups.GetHeadPosition(); pos;)
	{
		IUOLMessengerGroupPtr pGroup = listGroups.GetNext(pos);
		CString strGroupName = pGroup->GetName();

		m_wndComboGroups.AddString(strGroupName);
	}
	
	if (listGroups.GetCount() > 0)
	{
		m_wndComboGroups.SetCurSel(0);
	}
}


void CAuthorizeDialog::ResetAddContactFlag()
{
	m_bAddContact = FALSE;
}
