/* UOL messenger
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

#include "WizardPreferencesDialog.h"
#include "WizardFinishDialog.h"
#include "WizardPersonalizeAccountsDialog.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "UOLWizard.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"

#include "../skin/SolidBackgroundElement.h"

#include <interfaces/IUOLMessengerContactListSettings.h>



const CString CWizardPreferencesDialog::strMSNTimeAway = _T("5");
const CString CWizardPreferencesDialog::strICQTimeAway = _T("10");


CWizardPreferencesDialog::CWizardPreferencesDialog() :
	m_bkg(RGB(221, 237, 251)),
	bEditChange(TRUE)
{
	m_pNextPage = NULL;

	m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

void CWizardPreferencesDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardPreferencesDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardPreferencesDialog::GetName() const
{
	return "__WizardPreferencesInstance";
}

void CWizardPreferencesDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}

BOOL CWizardPreferencesDialog::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	/*Init*/
	m_pContactSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings();
	m_pGeneralSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings();
	m_pMessageSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings();
	m_pStatusSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetStatusSettings();
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	CWizardPersonalizeAccountsDialogPtr pWizardPersonalize = ap_dynamic_cast<CWizardPersonalizeAccountsDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardPersonalizeInstance"));
	pWizardPersonalize->SetAccountsList(m_pAccountsList);
	m_pNextPage = pWizardPersonalize;

	ATLASSERT(NULL != m_pWizard);
	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnPrevious(FALSE);
	/**/

	CString strPreSetMSNText, strPreSetICQText, strPreSetUOLText;
	CString strSortMethod;
	int nItemIndex = 0;
	int nUOLIndex = -1;

	//m_staticHeader.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PREFERENCES_HEADER));		
	//m_staticHeader.SetColor(RGB(0, 0, 0));
	//m_staticHeader.SetFont(m_BoldFont);

	strPreSetMSNText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_PREFERENCES_PRESET_MSN_TEXT");
	strPreSetICQText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_PREFERENCES_PRESET_ICQ_TEXT");
	strPreSetUOLText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_PREFERENCES_PRESET_UOL_TEXT");

	DoDataExchange(FALSE);

	nItemIndex = m_cmbPreset.AddString(strPreSetUOLText);
	m_cmbPreset.SetItemData(nItemIndex, CS_UOL_SETTINGS);
	nItemIndex = m_cmbPreset.AddString(strPreSetMSNText);
    m_cmbPreset.SetItemData(nItemIndex, CS_MSN_SETTTINGS);
	nItemIndex = m_cmbPreset.AddString(strPreSetICQText);
    m_cmbPreset.SetItemData(nItemIndex, CS_ICQ_SETTINGS);

    m_cmbPreset.SelectString(0, strPreSetUOLText);

	struct SortEntry
	{
		CString nResourceId;
		UINT nSortType;
	};
	SortEntry pSortEntries[] = 
	{
		{ m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_CONTACTS_PROPPAGE_SORT_METHOD_ALPHA"), IUOLMessengerContactListSettings::NAME },
		{ m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_CONTACTS_PROPPAGE_SORT_METHOD_STATUS"), IUOLMessengerContactListSettings::STATUS }
	};

	for (int nInd = 0; nInd < sizeof(pSortEntries) / sizeof(pSortEntries[0]); nInd++)
	{
		strSortMethod = pSortEntries[nInd].nResourceId;
		nItemIndex = m_cmbContactsOrder.AddString(strSortMethod);

		if (nItemIndex >= 0)
		{
			m_cmbContactsOrder.SetItemData(nItemIndex, pSortEntries[nInd].nSortType);
		}
	}       
	m_editInactivityMinutes.LimitText(2);
	m_editInactivityMinutes.SetReadOnly();

	m_updownTime.SetRange32(1, 60);
    m_updownTime.SetBuddy(m_editInactivityMinutes);
      
    SetPreferences(CS_UOL_SETTINGS);

	return TRUE;
}

void CWizardPreferencesDialog::OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	int nSelect;
	ClientSettings enSelected;

	DoDataExchange(TRUE);

	nSelect = m_cmbPreset.GetCurSel();
	enSelected = (ClientSettings) m_cmbPreset.GetItemData(nSelect);

	SetPreferences(enSelected);

	DoDataExchange(FALSE);
}

BOOL CWizardPreferencesDialog::OnBtnNext()
{
	DoDataExchange(TRUE);

	SaveContactsPreferences();
    SaveGeneralPreferences();
    SaveMessagesPreferences();
    SaveStatusPreferences();
    SaveAccountsPreferences();

	return TRUE;
}

CString CWizardPreferencesDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardPreferencesDialog::OnBtnCancel()
{
	IUOLMessengerAccountManagerPtr pAccountManager = CUIMApplication::GetApplication()->GetAccountManager();

	if (pAccountManager != NULL)
	{
		pAccountManager->SaveAccounts();
	}

	m_pWizard->EndDialog(IDOK);

	return FALSE;	

	//return TRUE;
}

LRESULT CWizardPreferencesDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, m_bkg);

	return (LRESULT) m_brushStatic.m_hBrush;
}

void CWizardPreferencesDialog::OnCheckChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(FALSE);

	m_cmbPreset.SetCurSel(-1);
}

void CWizardPreferencesDialog::OnRadioChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(FALSE);

	m_cmbPreset.SetCurSel(-1);
}

void CWizardPreferencesDialog::OnComboChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(FALSE);

	m_cmbPreset.SetCurSel(-1);
}

void CWizardPreferencesDialog::OnSpinChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	static BOOL bFirst = TRUE;

	DoDataExchange(FALSE);

	if (bEditChange == TRUE && bFirst == FALSE)
	{
        m_cmbPreset.SetCurSel(-1);
	}
	else
	{
		bFirst = FALSE;
		bEditChange = TRUE;
	}
}

BOOL CWizardPreferencesDialog::HasNextPage()	
{
	return TRUE;
}

BOOL CWizardPreferencesDialog::SetPreferences(ClientSettings enClient)	
{
	DoDataExchange(FALSE);

	SetContactsPreferences(enClient);
    SetGeneralPreferences(enClient);
    SetMessagesPreferences(enClient);
    SetStatusPreferences(enClient);
	SetAccountsPreferences(enClient);

	return TRUE;
}

void CWizardPreferencesDialog::SetContactsPreferences(ClientSettings enClient)
{
	m_btnSmallImage.SetCheck(FALSE);
    m_btnMediumImage.SetCheck(FALSE);
	m_btnGreatImage.SetCheck(FALSE);

	m_btnSecurityLow.SetCheck(FALSE);
	m_btnSecurityMedium.SetCheck(FALSE);
	m_btnSecurityHigh.SetCheck(FALSE);

	switch (enClient)
	{
        case CS_UOL_SETTINGS:
			SetUOLSortType(m_pContactSettings->GetSortType());
			m_cmbContactsOrder.SetCurSel(m_pContactSettings->GetSortType());
			m_btnContactGroup.SetCheck(m_pContactSettings->IsShowGroupsEnabled());
			m_btnNotifyOnOffContacts.SetCheck(m_pContactSettings->GetNotifyOnline() ||
											  m_pContactSettings->GetNotifyOffline());
            
			SetUOLSizeImage(m_pContactSettings->GetSizeImage());

            SetUOLSecurityLevel(m_pContactSettings->GetSecurityLevel());

		break;

        case CS_ICQ_SETTINGS:
			m_cmbContactsOrder.SetCurSel(1);
			m_btnContactGroup.SetCheck(FALSE);
			m_btnNotifyOnOffContacts.SetCheck(TRUE);
			m_btnSmallImage.SetCheck(TRUE);
			m_btnSecurityMedium.SetCheck(TRUE);
		break;

		case CS_MSN_SETTTINGS:
			m_cmbContactsOrder.SetCurSel(1);
			m_btnContactGroup.SetCheck(FALSE);
			m_btnNotifyOnOffContacts.SetCheck(TRUE);
			m_btnSmallImage.SetCheck(TRUE);
			m_btnSecurityMedium.SetCheck(TRUE);
		break;
	}
}

void CWizardPreferencesDialog::SetGeneralPreferences(ClientSettings enClient)
{
	switch (enClient)
	{
        case CS_UOL_SETTINGS:
			m_btnAutoOpen.SetCheck(m_pGeneralSettings->IsAutoStartupEnabled());
			m_btnOpenBrowser.SetCheck(m_pGeneralSettings->IsOpenBrowserEnabled());
			m_btnTouchSounds.SetCheck(m_pGeneralSettings->IsEnableSound());
		break;

        case CS_ICQ_SETTINGS:
        case CS_MSN_SETTTINGS:
			m_btnAutoOpen.SetCheck(TRUE);
			m_btnOpenBrowser.SetCheck(FALSE);
			m_btnTouchSounds.SetCheck(TRUE);
		break;
	}
}

void CWizardPreferencesDialog::SetMessagesPreferences(ClientSettings enClient)
{
	switch (enClient)
	{
        case CS_UOL_SETTINGS:
			m_btnHistory.SetCheck(m_pMessageSettings->IsLogMessagesEnabled());
            m_btnTabWindow.SetCheck(m_pMessageSettings->IsOnNewMsgWindowEnabled());
		break;

        case CS_ICQ_SETTINGS:
        case CS_MSN_SETTTINGS:
			m_btnHistory.SetCheck(FALSE);
            m_btnTabWindow.SetCheck(FALSE);
		break;
	}
}

void CWizardPreferencesDialog::SetStatusPreferences(ClientSettings enClient)
{
    UINT nTime;
    
	bEditChange = FALSE;
	
    switch (enClient)
	{
        case CS_UOL_SETTINGS:
			m_btnAutoAway.SetCheck(m_pStatusSettings->IsAutoAwayEnabled());
			nTime = m_pStatusSettings->GetTimeout();
			m_updownTime.SetPos(nTime);
		break;

        case CS_MSN_SETTTINGS:
			m_btnAutoAway.SetCheck(TRUE);
			m_editInactivityMinutes.SetWindowText(strMSNTimeAway);
		break;

        case CS_ICQ_SETTINGS:
			m_btnAutoAway.SetCheck(TRUE);
			m_editInactivityMinutes.SetWindowText(strICQTimeAway);
		break;
	}
}

void CWizardPreferencesDialog::SetAccountsPreferences(ClientSettings enClient)
{
    m_btnAutoConnect.SetCheck(TRUE);
}

void CWizardPreferencesDialog::SaveContactsPreferences()
{
	UINT nSortType = (UINT) m_cmbContactsOrder.GetItemData(m_cmbContactsOrder.GetCurSel());

	m_pContactSettings->SetSortType(nSortType);

	m_pContactSettings->EnableShowGroups(m_btnContactGroup.GetCheck());
	m_pContactSettings->SetNotifyOnline(m_btnNotifyOnOffContacts.GetCheck());
	m_pContactSettings->SetNotifyOffline(m_btnNotifyOnOffContacts.GetCheck());

	if (m_btnSmallImage.GetCheck())
	{
		m_pContactSettings->SetSizeImage(IUOLMessengerContactListSettings::SMALL_IMAGE);
	}
	else if (m_btnMediumImage.GetCheck())
	{
		m_pContactSettings->SetSizeImage(IUOLMessengerContactListSettings::MEDIUM_IMAGE);
	}
	else if (m_btnGreatImage.GetCheck())
	{
		m_pContactSettings->SetSizeImage(IUOLMessengerContactListSettings::GREAT_IMAGE);
	}


	if (m_btnSecurityLow.GetCheck())
	{
        m_pContactSettings->SetSecurityLevel(IUOLMessengerContactListSettings::SL_RECEIVE_FROM_ANY_USER);
	}
	else if (m_btnSecurityMedium.GetCheck())
	{
        m_pContactSettings->SetSecurityLevel(IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE);
	}
	else if (m_btnSecurityHigh.GetCheck())
	{
        m_pContactSettings->SetSecurityLevel(IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST);
	}
}

void CWizardPreferencesDialog::SaveGeneralPreferences()
{
    m_pGeneralSettings->EnableAutoStartup(m_btnAutoOpen.GetCheck());

    m_pGeneralSettings->EnableOpenBrowser(m_btnOpenBrowser.GetCheck());

	m_pGeneralSettings->EnableSound(m_btnTouchSounds.GetCheck());
}

void CWizardPreferencesDialog::SaveMessagesPreferences()
{
	m_pMessageSettings->EnableLogMessages(m_btnHistory.GetCheck());
	m_pMessageSettings->EnableOnNewMsgWindow(m_btnTabWindow.GetCheck());
}

void CWizardPreferencesDialog::SaveStatusPreferences()
{
	CString strTime;
	m_editInactivityMinutes.GetWindowText(strTime);

	UINT nMinutes = ::_tstoi(strTime);

	if ((nMinutes > 0))
	{
		m_pStatusSettings->SetTimeout(nMinutes);
	}

	m_pStatusSettings->EnableAutoAway(m_btnAutoAway.GetCheck());
}

void CWizardPreferencesDialog::SaveAccountsPreferences()
{
    if(m_pAccountsList) {

	IUOLMessengerAccountPtr pAccount;

        for (POSITION pos = m_pAccountsList->GetHeadPosition(); pos; m_pAccountsList->GetNext(pos)) 	{
        pAccount = m_pAccountsList->GetAt(pos);

        pAccount->EnableAutoConnect(m_btnAutoConnect.GetCheck());
	}
    }
}

void CWizardPreferencesDialog::SetUOLSortType(UINT nSecurityLevel)
{
    switch(nSecurityLevel)
	{
	case IUOLMessengerContactListSettings::NAME:
		m_cmbContactsOrder.SetCurSel(0);
		break;

	case IUOLMessengerContactListSettings::STATUS:
		m_cmbContactsOrder.SetCurSel(1);
		break;
	}
}

void CWizardPreferencesDialog::SetUOLSecurityLevel(UINT nSecurityLevel)
{
	switch (nSecurityLevel)
	{
        case IUOLMessengerContactListSettings::SL_RECEIVE_FROM_ANY_USER:
			m_btnSecurityLow.SetCheck(TRUE);
			break;

		case IUOLMessengerContactListSettings::SL_CONFIRM_RECEIVE:
			m_btnSecurityMedium.SetCheck(TRUE);
			break;

		case IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST:
			m_btnSecurityHigh.SetCheck(TRUE);
			break;
	}
}


void CWizardPreferencesDialog::SetUOLSizeImage(UINT nSecurityLevel)
{
	switch(nSecurityLevel)
	{
	case IUOLMessengerContactListSettings::SMALL_IMAGE:
        m_btnSmallImage.SetCheck(TRUE);
		break;

	case IUOLMessengerContactListSettings::MEDIUM_IMAGE:
        m_btnMediumImage.SetCheck(TRUE);
		break;

	case IUOLMessengerContactListSettings::GREAT_IMAGE:
        m_btnGreatImage.SetCheck(TRUE);
		break;
	}
}
