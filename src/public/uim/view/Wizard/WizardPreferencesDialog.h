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


#pragma once

#include "stdafx.h"
#include "../../resource.h"

#include "UOLPropPageImpl.h"
#include "../transparentstatic.h"
#include "../Skin/COLORREF.h"

#include <interfaces/IUOLMessengerProfileManager.h>
#include <interfaces/IUOLMessengerSettingsManager.h>


class CWizardPreferencesDialog : 
	public CUOLPropPageImpl,
    public CWinDataExchange<CWizardPreferencesDialog>
{
public:
    enum { IDD = IDD_DIALOG_WIZARD_PREFERENCES };
	DECLARE_ELEMENT_NAME("WizardPreferencesDialog");

    CWizardPreferencesDialog();

    BEGIN_MSG_MAP(CWizardPreferencesDialog)
		COMMAND_HANDLER_EX(IDC_COMBO_WIZARD_PREFERENCES_PRESET, LBN_SELCHANGE, OnSelectChange)
		COMMAND_HANDLER_EX(IDC_COMBO_WIZARD_PREFERENCES_CONTACTS_ORDER, CBN_SELCHANGE, OnComboChange)
		COMMAND_HANDLER_EX(IDC_EDIT_WIZARD_PREFERENCES_TIME_AUTO_AWAY, EN_CHANGE, OnSpinChange)
		COMMAND_RANGE_HANDLER_EX(IDC_CHECK_WIZARD_PREFERENCES_AUTO_CONNECT, IDC_CHECK_WIZARD_PREFEFENCES_WINDOW_NOTIFY, OnCheckChange)
		COMMAND_RANGE_HANDLER_EX(IDC_RADIO_WIZARD_PREFERENCES_SECURITY_RECEIVE_ALL, IDC_RADIO_WIZARD_PREFERENCES_GREAT_IMAGE, OnRadioChange)
		COMMAND_ID_HANDLER_EX(IDC_SPIN_WIZARD_PREFERENCES_INACTIVITY_MINUTES, OnSpinChange)

		MSG_WM_CTLCOLORSTATIC(OnCtlColor)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(CUOLPropPageImpl)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CWizardPreferencesDialog)
		DDX_CONTROL_HANDLE(IDC_COMBO_WIZARD_PREFERENCES_PRESET, m_cmbPreset)
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_AUTO_AWAY, m_btnAutoAway)
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_AUTO_OPEN, m_btnAutoOpen)
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_OPEN_NAVIGATOR, m_btnOpenBrowser)
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_AUTO_CONNECT, m_btnAutoConnect)
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_HISTORY, m_btnHistory)
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_SHOW_GROUPS, m_btnContactGroup)
		DDX_CONTROL_HANDLE(IDC_COMBO_WIZARD_PREFERENCES_CONTACTS_ORDER, m_cmbContactsOrder)
		DDX_CONTROL_HANDLE(IDC_RADIO_WIZARD_PREFERENCES_SECURITY_RECEIVE_ALL, m_btnSecurityLow)
		DDX_CONTROL_HANDLE(IDC_RADIO_WIZARD_PREFERENCES_SECURITY_ASK_BEFORE_RECEIVE, m_btnSecurityMedium)
		DDX_CONTROL_HANDLE(IDC_RADIO_WIZARD_PREFERENCES_SECURITY_RECEIVE_ONLY_FROM_CONTACT_LIST, m_btnSecurityHigh)
		DDX_CONTROL_HANDLE(IDC_EDIT_WIZARD_PREFERENCES_TIME_AUTO_AWAY, m_editInactivityMinutes);
		DDX_CONTROL_HANDLE(IDC_SPIN_WIZARD_PREFERENCES_INACTIVITY_MINUTES, m_updownTime);
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFEFENCES_WINDOW_NOTIFY, m_btnNotifyOnOffContacts);
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_TOUCH_SOUND, m_btnTouchSounds);
		DDX_CONTROL_HANDLE(IDC_RADIO_WIZARD_PREFERENCES_SMALL_IMAGE, m_btnSmallImage);
		DDX_CONTROL_HANDLE(IDC_RADIO_WIZARD_PREFERENCES_MEDIUM_IMAGE, m_btnMediumImage);
		DDX_CONTROL_HANDLE(IDC_RADIO_WIZARD_PREFERENCES_GREAT_IMAGE, m_btnGreatImage);
		DDX_CONTROL_HANDLE(IDC_CHECK_WIZARD_PREFERENCES_TAB_WINDOW, m_btnTabWindow);
	END_DDX_MAP()

	void SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList);
protected:
	int GetDlgID(){	return IDD_DIALOG_WIZARD_PREFERENCES; }

	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);	
	CString GetName() const;

	void SetUOLWizard(CUOLWizard* pWizard);

	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);

	LRESULT OnCtlColor(HDC hdc, HWND hwnd);

	void OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnCheckChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnRadioChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnComboChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnSpinChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);

	CString GetCancelBtnText();
	BOOL OnBtnCancel();
	BOOL OnBtnNext();
	BOOL HasNextPage();

private:

	enum ClientSettings
	{
		CS_MSN_SETTTINGS,
		CS_ICQ_SETTINGS,
		CS_UOL_SETTINGS
	};

    BOOL SetPreferences(ClientSettings enClient);
	void SetContactsPreferences(ClientSettings enClient);
    void SetGeneralPreferences(ClientSettings enClient);
    void SetMessagesPreferences(ClientSettings enClient);		
    void SetStatusPreferences(ClientSettings enClient);
    void SetAccountsPreferences(ClientSettings enClient);

	void SaveContactsPreferences();
    void SaveGeneralPreferences();
    void SaveMessagesPreferences();
    void SaveStatusPreferences();
    void SaveAccountsPreferences();

    void SetUOLSortType(UINT nSecurityLevel);
    void SetUOLSecurityLevel(UINT nSecurityLevel);
    void SetUOLSizeImage(UINT nSecurityLevel);

private:
	static const CString strMSNTimeAway, strICQTimeAway;

    AccountsList*	m_pAccountsList;
	
	IUOLMessengerContactListSettingsPtr m_pContactSettings;
	IUOLMessengerGeneralSettingsPtr m_pGeneralSettings;
	IUOLMessengerMessageSettingsPtr m_pMessageSettings;
	IUOLMessengerStatusSettingsPtr m_pStatusSettings;

	CComboBox		m_cmbPreset;
	CComboBox		m_cmbContactsOrder;

	CUpDownCtrl		m_updownTime;

	CEdit		m_editInactivityMinutes;

	BOOL		bEditChange;

	CButton		m_btnAutoAway;
	CButton		m_btnAutoOpen;
	CButton		m_btnAutoConnect;
	CButton		m_btnHistory;
	CButton		m_btnContactGroup;
	CButton		m_btnSmallImage;
	CButton		m_btnMediumImage;
	CButton		m_btnGreatImage;
	CButton		m_btnSecurityLow;
	CButton		m_btnSecurityMedium;
	CButton		m_btnSecurityHigh;
	CButton		m_btnOpenBrowser;
	CButton		m_btnTouchSounds;
	CButton		m_btnNotifyOnOffContacts;
	CButton		m_btnTabWindow;

	CTransparentStatic		m_staticHeader;
	CTransparentStatic		m_staticContacts;

	WTL::CBrush		m_brushStatic;
	CCOLORREF		m_bkg;
};

MAKEAUTOPTR(CWizardPreferencesDialog);