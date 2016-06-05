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
#include ".\InfoWindow.h"

#include "../resource.h"
#include "../controller/UIMApplication.h"
#include "../controller/InfoWindow/InfoWindowController.h"


CInfoWindow::CInfoWindow(CMainFrame* pParentFrame, IUOLMessengerContactPtr pContact) :
	m_pParentFrame(pParentFrame),
	m_pContact(pContact)
{
	ATLASSERT(m_pParentFrame != NULL);
}


CInfoWindow::~CInfoWindow()
{
	//ATLTRACE(_T("CInfoWindow::~CInfoWindow()\n"));
}


IUOLMessengerContactPtr CInfoWindow::GetContact()
{
	return m_pContact;
}


void CInfoWindow::OnContactGotInfo(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strInfo)
{
	if ((0 == strContact.Compare(m_pContact->GetName())) &&
		(0 == pAccount->GetUserId().Compare(m_pContact->GetAccount()->GetUserId())) &&
		(0 == pAccount->GetProtocolId().Compare(m_pContact->GetAccount()->GetProtocolId())))
	{
		CONTACTINFOEVENT cie;
		cie.strInfo = strInfo;

		SendMessage(WM_USER_CONTACT_INFO_RECEIVED, (WPARAM) &cie, NULL);
	}
}


LRESULT CInfoWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	ATLASSERT(m_pContact.get() != NULL);

	if (m_pContact.get() != NULL)
	{
		IUOLMessengerAccountPtr pAccount = m_pContact->GetAccount();

		ATLASSERT(pAccount != NULL);

		if (pAccount != NULL)
		{
			// Create details window
			m_pDetailsWindow = new CDetailsWindow();
			m_pDetailsWindow->Create(m_hWnd);

			// Register as remote contact list observer
			CUIMApplication::GetApplication()->GetRemoteContactList()->RegisterObserver(this);

			// Set title...
			CString strAlias = m_pContact->GetAlias();
			if (strAlias.IsEmpty())
			{
				strAlias = m_pContact->GetName();
			}

			CString strTitle;
			strTitle.Format(_T("%s - %s"), strAlias, m_pContact->GetAccount()->GetProtocolName());

			m_pParentFrame->SetCaptionText(strTitle);

			// Get contact info
			CUIMApplication::GetApplication()->GetRemoteContactList()->GetContactInfo(pAccount, (LPCTSTR) m_pContact->GetName());
		}
	}

	return 0L;
}


void CInfoWindow::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	// Unregister as remote contact list observer...
	CUIMApplication::GetApplication()->GetRemoteContactList()->UnregisterObserver(this);

	if (m_pDetailsWindow != NULL)
	{
		m_pDetailsWindow->DestroyWindow();
	}

	SetMsgHandled(FALSE);
}


LRESULT CInfoWindow::OnContactInfoReceivedEvent(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPCONTACTINFOEVENT pcie = (LPCONTACTINFOEVENT) wParam;

	if (pcie != NULL)
	{
		if ((m_pDetailsWindow != NULL) && (m_pDetailsWindow->IsWindow()))
		{
			m_pDetailsWindow->SetDetailsInfo(pcie->strInfo);
		}
	}

	return 0L;
}


void CInfoWindow::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);

	m_pDetailsWindow->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
}
