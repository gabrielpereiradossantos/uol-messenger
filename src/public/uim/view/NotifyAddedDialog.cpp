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
#include "NotifyAddedDialog.h"

#include "ContactDialog.h"
#include "../controller/NotifyWindow/NotifyWindowController.h"
#include "../controller/UIMApplication.h"


CNotifyAddedDialog::CNotifyAddedDialog(IUOLMessengerContactPtr pContact, BOOL bAlreadyAdded) :
m_pContact(pContact),
m_bAlreadyAdded(bAlreadyAdded)
{
}

CNotifyAddedDialog::~CNotifyAddedDialog(void)
{
}

LRESULT CNotifyAddedDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	CString strTitle;
	CString strLabel;

	strTitle = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
	strLabel.Format(IDS_YOU_WERE_ADDED, m_pContact->GetName());

	CNotifyAddedDialog::SetWindowText(strTitle);
	GetDlgItem(IDC_STATIC_LABEL).SetWindowText(strLabel);

	UpdateUI();

	CenterWindow();

	return 0L;
}

LRESULT CNotifyAddedDialog::OnClose()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DestroyWindow();
	return 0L;
}

void CNotifyAddedDialog::OnDestroy()
{
	if (FALSE == m_bAlreadyAdded)
	{
		// Remove automatically added buddy
		IUOLMessengerAccountPtr pAccount = m_pContact->GetAccount();
		
		if (pAccount)
		{
			CIMProtocol* pProtocol = pAccount->GetProtocol();

			if (pProtocol && pProtocol->AutoAddContactOnAuthorization())
			{
				CString strDefaultGroup = pProtocol->GetDefaultContactGroup().c_str();

				if (FALSE == strDefaultGroup.IsEmpty())
				{
					IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();
					ATLASSERT(pContactList);

					IUOLMessengerContactPtr pRemovedContact = pContactList->RemoveContact(pAccount, m_pContact->GetName(), strDefaultGroup);
					ATLASSERT(pRemovedContact);
				}
			}
		}
	}
}

void CNotifyAddedDialog::OnFinalMessage(HWND /*hWnd*/)
{
	// Remove from controller list
	CNotifyWindowController::GetInstance()->RemoveWindow(this);
}

void CNotifyAddedDialog::OnBnClickedButtonOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DestroyWindow();
}

void CNotifyAddedDialog::OnBnClickedAddContact(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_bAlreadyAdded = CUIMApplication::GetApplication()->GetUIManager()->ShowContactDlg(m_pContact->GetAccount(), m_pContact->GetName());

	UpdateUI();
}

void CNotifyAddedDialog::OnBnClickedViewInfo(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	CUIMApplication::GetApplication()->GetUIManager()->LaunchInfoWindow(m_pContact);
}

IUOLMessengerContactPtr CNotifyAddedDialog::GetContact() const
{
	return m_pContact;
}

void CNotifyAddedDialog::UpdateUI()
{
	CWindow button = GetDlgItem(IDC_BUTTON_ADD_CONTACT);

	button.EnableWindow(FALSE == m_bAlreadyAdded);
}