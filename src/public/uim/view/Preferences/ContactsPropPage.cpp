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
 
// ContactPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "ContactsPropPage.h"

#include "../../controller/UIMApplication.h"
#include "../skin/ContactNodeLayoutBitmapElement.h"
#include "../../controller/UIManager.h"


// CContactsPropPage dialog

CContactsPropPage::CContactsPropPage() :
	CPreferencesPropPage(IDD_DIALOG_CONTACTS),
	m_dwToNotify(0)
{	
}

CContactsPropPage::~CContactsPropPage()
{
}


BOOL CContactsPropPage::OnIdle()
{
	return FALSE;
}


LRESULT CContactsPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	m_pSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings();

	if (m_pSettings != NULL)
	{
		CButton btnSizeImage;
        CButton btnSecurityLevel;
        CButton btnSortType;

		m_btnShowOffline = GetDlgItem(IDC_CHECK_SHOW_OFFLINE);
		m_btnShowGroups = GetDlgItem(IDC_CHECK_SHOW_GROUPS);
		m_btnNotifyOffline = GetDlgItem(IDC_CHECK_NOTIFY_OFFLINE);
		m_btnNotifyOnline = GetDlgItem(IDC_CHECK_NOTIFY_ONLINE);
		m_btnTopMost = GetDlgItem(IDC_CHECK_TOPMOST);
		m_btnGroupByAccounts = GetDlgItem(IDC_CHECK_GROUP_BY_ACCOUNTS);

		m_imageSize = m_pSettings->GetSizeImage();
        m_nSortType = m_pSettings->GetSortType();
		m_nSecurityLevel = m_pSettings->GetSecurityLevel();
		btnSizeImage = GetBtnSizeImage(m_imageSize);
        btnSortType =  GetBtnSortType(m_nSortType);
		btnSecurityLevel = GetBtnSecurityLevel(m_nSecurityLevel);
		
		m_btnShowOffline.SetCheck(m_pSettings->IsShowOfflineEnabled());
		m_btnShowGroups.SetCheck(m_pSettings->IsShowGroupsEnabled());

		m_btnNotifyOffline.SetCheck(m_pSettings->GetNotifyOffline());
		m_btnNotifyOnline.SetCheck(m_pSettings->GetNotifyOnline());
		m_btnTopMost.SetCheck(m_pSettings->IsTopMost());
		
		btnSizeImage.SetCheck(TRUE);
        btnSortType.SetCheck(TRUE);
		btnSecurityLevel.SetCheck(TRUE);
		
		IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
				ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pSettings);
		
		m_btnGroupByAccounts.SetCheck(pContactListSettings2->GetGroupByAccounts());
		
		if (!pContactListSettings2->IsShowGroupsEnabled())
		{
			m_btnGroupByAccounts.EnableWindow(FALSE);
		}
	}

	CUIMApplication::GetApplication()->AddIdleHandler(this);
	
	m_dwToNotify = 0;

    // Create "ContactNodeLayoutCfg" component
    CWindow group = GetDlgItem(IDC_STATIC_CONTACT_NODE_LAYOUT);
    CWindow txt = GetDlgItem(IDC_STATIC_CONTACT_NODE_LAYOUT_EXPLANATION);
    CRect grpRect;
    CRect wndRect;
    CRect txtRect;
    group.GetWindowRect(&grpRect);
    txt.GetWindowRect(&txtRect);
    GetWindowRect(&wndRect);
    CRect rectLayout;
	rectLayout.SetRect((grpRect.left - wndRect.left) + 7, 
			(grpRect.top - wndRect.top) + 17, 
			(grpRect.right - wndRect.left) - 7, 
			(grpRect.bottom - wndRect.top - (txtRect.bottom - txtRect.top)) - 17);
	
	m_ContactNodeLayoutCfg.Create(m_hWnd, rectLayout, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	
    // Get bitmaps from skin
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	CContactNodeLayoutBitmapElementPtr pContactNodeLayoutBitmap = ap_dynamic_cast<CContactNodeLayoutBitmapElementPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__ContactNodeLayoutBitmapInstance"));
    CString bmpAlias;
    CString bmpAvatar;
    CString bmpStatus;

	if (pContactNodeLayoutBitmap)
	{
		bmpAlias = pContactNodeLayoutBitmap->GetBitmapAliasPath();
        bmpAvatar = pContactNodeLayoutBitmap->GetBitmapAvatarPath();
        bmpStatus = pContactNodeLayoutBitmap->GetBitmapStatusPath();
	}

	m_ContactNodeLayoutCfg.AddElement(IUOLMessengerContactListSettings2::AVATAR_ELEMENT, 
		"Avatar", bmpAvatar, FALSE);
	m_ContactNodeLayoutCfg.AddElement(IUOLMessengerContactListSettings2::ALIAS_ELEMENT, 
		"Nome", bmpAlias, TRUE);
	m_ContactNodeLayoutCfg.AddElement(IUOLMessengerContactListSettings2::STATUS_ELEMENT, 
		"Status", bmpStatus, FALSE);
	m_ContactNodeLayoutCfg.CreateControls();
	m_ContactNodeLayoutCfg.BringWindowToTop();

    //Get layout stored and set it into component
    IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
				ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pSettings);
	m_sContactNodeLayout = pContactListSettings2->GetContactNodeLayout();

	m_ContactNodeLayoutCfg.SetContactNodeLayout(m_sContactNodeLayout);

	return 0L;
}


void CContactsPropPage::OnDestroy()
{
	CUIMApplication::GetApplication()->RemoveIdleHandler(this);

	SetMsgHandled(FALSE);
}


void CContactsPropPage::OnRadioContactsSortMethodClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CButton btnSortMethod;
	btnSortMethod = hwndCtrl;

    if ( 1 == btnSortMethod.GetCheck() )
	{
		switch (nID)
		{ 
		case IDC_RADIO_SORT_ALPHA:
			m_nSortType = IUOLMessengerContactListSettings::NAME;
			break;

		case IDC_RADIO_SORT_STATUS:
			m_nSortType = IUOLMessengerContactListSettings::STATUS;
			break;
		}

		if (m_nSortType != m_pSettings->GetSortType())
		{
			m_dwToNotify |= NL_SORT_TYPE;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		}
	}
}


void CContactsPropPage::OnCheckButtonClicked(UINT uCode, int nID, HWND hwndCtrl)
{
	if (IDC_CHECK_SHOW_OFFLINE == nID)
	{
		//m_pSettings->EnableShowOffline(m_btnShowOffline.GetCheck() ? TRUE : FALSE);
		m_dwToNotify |= NL_SHOW_OFFLINE;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_OFFLINE);
	}
	else if (IDC_CHECK_SHOW_GROUPS == nID)
	{
		//m_pSettings->EnableShowGroups(m_btnShowGroups.GetCheck() ? TRUE : FALSE);
		m_dwToNotify |= NL_SHOW_GROUPS;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_GROUPS);
		
		if (m_btnShowGroups.GetCheck())
		{
			m_btnGroupByAccounts.EnableWindow(TRUE);
		}
		else
		{
			m_btnGroupByAccounts.EnableWindow(FALSE);
			
			/*
			if (m_btnGroupByAccounts.GetCheck())
			{
				m_btnGroupByAccounts.SetCheck(FALSE);
				
				m_dwToNotify |= NL_GROUP_BY_ACCOUNTS;
				GetParent().SendMessage(WM_USER_PREFS_CHANGED);
			}
			*/
		}
	}
	else if (IDC_CHECK_NOTIFY_OFFLINE == nID)
	{
		//if (m_btnNotifyOffline.GetCheck())
		//{
		//	m_pSettings->SetNotifyOffline(TRUE);
		//}
		//else
		//{
		//	m_pSettings->SetNotifyOffline(FALSE);
		//}

		m_dwToNotify |= NL_NOTIFY_TYPE;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_NOTIFY_TYPE);
	}
	else if (IDC_CHECK_NOTIFY_ONLINE == nID)
	{
		//if (m_btnNotifyOnline.GetCheck())
		//{
		//	m_pSettings->SetNotifyOnline(TRUE);
		//}
		//else
		//{
		//	m_pSettings->SetNotifyOnline(FALSE);
		//}

		m_dwToNotify |= NL_NOTIFY_TYPE;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_NOTIFY_TYPE);
	}
	else if (IDC_CHECK_GROUP_BY_ACCOUNTS == nID)
	{
		m_dwToNotify |= NL_GROUP_BY_ACCOUNTS;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
	}
}


void CContactsPropPage::OnRadioImageSizeClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CButton btnImage;
	btnImage = hwndCtrl;

	if ( 1 == btnImage.GetCheck() )
	{
		switch (nID)
		{
		case IDC_RADIO_SMALL_IMAGE:
			m_imageSize = IUOLMessengerContactListSettings::SMALL_IMAGE;
			//m_pSettings->SetSizeImage(IUOLMessengerContactListSettings::SMALL_IMAGE);
			break;

		case IDC_RADIO_MEDIUM_IMAGE:
			m_imageSize = IUOLMessengerContactListSettings::MEDIUM_IMAGE;
			//m_pSettings->SetSizeImage(IUOLMessengerContactListSettings::MEDIUM_IMAGE);
			break;

		case IDC_RADIO_GREAT_IMAGE:
			m_imageSize = IUOLMessengerContactListSettings::GREAT_IMAGE;
			//m_pSettings->SetSizeImage(IUOLMessengerContactListSettings::GREAT_IMAGE);
			break;
		}

		m_dwToNotify |= NL_SIZEIMAGE;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SIZEIMAGE);
	}
}


void CContactsPropPage::OnClickTopMost(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
    //m_pSettings->SetTopMost(m_btnTopMost.GetCheck());

	m_dwToNotify |= NL_TOPMOST;
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);
	//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_TOPMOST);
}


void CContactsPropPage::OnRadioSecurityLevelClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CButton btnSecutity;
	btnSecutity = hwndCtrl;

	if ( 1 == btnSecutity.GetCheck() )
	{
		//UINT nSecurityLevel = IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST;

		if (nID == IDC_RADIO_SECURITY_RECEIVE_ALL)
		{
			m_nSecurityLevel = IUOLMessengerContactListSettings::SL_RECEIVE_FROM_ANY_USER;
		}
		else if (nID == IDC_RADIO_SECURITY_ASK_BEFORE_RECEIVE)
		{
			m_nSecurityLevel = IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE;
		}
		else
		{
			ATLASSERT(nID == IDC_RADIO_SECURITY_RECEIVE_ONLY_FROM_CONTACT_LIST);
			m_nSecurityLevel = IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST;
		}

		//m_pSettings->SetSecurityLevel(nSecurityLevel);
		m_dwToNotify |= NL_SECURITY_LEVEL;
		GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		//m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SECURITY_LEVEL);
	}
}

LRESULT CContactsPropPage::OnContactNodeLayoutChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_dwToNotify |= NL_CONTACT_NODE_LAYOUT;
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);

    return TRUE;
}

HWND CContactsPropPage::GetBtnSizeImage(IUOLMessengerContactListSettings::SIZE_IMAGE nSizeImage)
{
	UINT nId;

	switch (nSizeImage)
	{
        case IUOLMessengerContactListSettings::SMALL_IMAGE:
			nId = IDC_RADIO_SMALL_IMAGE;
			break;

		case IUOLMessengerContactListSettings::MEDIUM_IMAGE:
			nId = IDC_RADIO_MEDIUM_IMAGE;
			break;

		case IUOLMessengerContactListSettings::GREAT_IMAGE:
			nId = IDC_RADIO_GREAT_IMAGE;
			break;

		default:
			nId = -1;
	}

	if (-1 != nId)
	{
		return GetDlgItem(nId);
	}

	return NULL;
}

HWND CContactsPropPage::GetBtnSortType(UINT nSortType)
{
	UINT nId;

	switch (nSortType)
	{
        case IUOLMessengerContactListSettings::NAME:
            nId = IDC_RADIO_SORT_ALPHA;
			break;

		case IUOLMessengerContactListSettings::STATUS:
            nId = IDC_RADIO_SORT_STATUS;
			break;

        default:
			nId = -1;
	}

	if (-1 != nId)
	{
		return GetDlgItem(nId);
	}

	return NULL;
}


HWND CContactsPropPage::GetBtnSecurityLevel(UINT nSecurityLevel)
{
	UINT nId;

	switch (nSecurityLevel)
	{
        case IUOLMessengerContactListSettings::SL_RECEIVE_FROM_ANY_USER:
			nId = IDC_RADIO_SECURITY_RECEIVE_ALL;
			break;

		case IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE:
			nId = IDC_RADIO_SECURITY_ASK_BEFORE_RECEIVE;
			break;

		case IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST:
			nId = IDC_RADIO_SECURITY_RECEIVE_ONLY_FROM_CONTACT_LIST;
			break;

		default:
			nId = -1;
	}

	if (-1 != nId)
	{
		return GetDlgItem(nId);
	}

	return NULL;
}


LRESULT CContactsPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}

void CContactsPropPage::OnApply()
{
	if ( NL_SORT_TYPE & m_dwToNotify )
	{
		m_pSettings->SetSortType(m_nSortType);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SORT_TYPE);
	}

	if ( NL_SHOW_OFFLINE & m_dwToNotify )
	{
		m_pSettings->EnableShowOffline(m_btnShowOffline.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_OFFLINE);
	}

	if ( NL_SHOW_GROUPS & m_dwToNotify )
	{
		m_pSettings->EnableShowGroups(m_btnShowGroups.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SHOW_GROUPS);
	}

	if ( NL_NOTIFY_TYPE	& m_dwToNotify )
	{
		m_pSettings->SetNotifyOffline(m_btnNotifyOffline.GetCheck() ? TRUE : FALSE);
		m_pSettings->SetNotifyOnline(m_btnNotifyOnline.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_NOTIFY_TYPE);
	}

	if ( NL_TOPMOST & m_dwToNotify )
	{
		m_pSettings->SetTopMost(m_btnTopMost.GetCheck());
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_TOPMOST);
	}

	if ( NL_SIZEIMAGE & m_dwToNotify ) 
	{
		m_pSettings->SetSizeImage(m_imageSize);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SIZEIMAGE);
	}

	if ( NL_SECURITY_LEVEL & m_dwToNotify )
	{
		m_pSettings->SetSecurityLevel(m_nSecurityLevel);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_SECURITY_LEVEL);
	}

	if ( NL_GROUP_BY_ACCOUNTS & m_dwToNotify )
	{
		IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
				ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pSettings);
		
		pContactListSettings2->SetGroupByAccounts(m_btnGroupByAccounts.GetCheck() ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerContactListSettings::CL_GROUP_BY_ACCOUNTS);
	}

    if ( NL_CONTACT_NODE_LAYOUT & m_dwToNotify )
	{
		IUOLMessengerContactListSettings2Ptr pContactListSettings2 = 
				ap_static_cast<IUOLMessengerContactListSettings2Ptr>(m_pSettings);
		
        m_sContactNodeLayout = m_ContactNodeLayoutCfg.GetContactNodeLayout();
		pContactListSettings2->SetContactNodeLayout(m_sContactNodeLayout);
		pContactListSettings2->NotifyEventListeners(IUOLMessengerContactListSettings::CL_CONTACT_NODE_LAYOUT);
	}

	m_dwToNotify = 0;
}
