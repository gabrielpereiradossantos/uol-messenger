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

#include "PreferencesPropPage.h"
#include "../../resource.h"
#include <interfaces/IUOLMessengerContactListSettings.h>
#include "ContactNodeLayoutCfg.h"

// CContactsPropPage dialog

class CContactsPropPage : public CPreferencesPropPage,
						  public CIdleHandler
{
public:
	CContactsPropPage();
	virtual ~CContactsPropPage();

	virtual void OnApply();

protected:
	virtual BOOL OnIdle();

	BEGIN_MSG_MAP(CContactsPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		//MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		COMMAND_RANGE_HANDLER_EX(IDC_RADIO_SMALL_IMAGE, IDC_RADIO_GREAT_IMAGE, OnRadioImageSizeClicked) 
        COMMAND_RANGE_HANDLER_EX(IDC_RADIO_SORT_ALPHA, IDC_RADIO_SORT_STATUS, OnRadioContactsSortMethodClicked) 

		COMMAND_ID_HANDLER_EX(IDC_CHECK_SHOW_OFFLINE, OnCheckButtonClicked) 
		COMMAND_ID_HANDLER_EX(IDC_CHECK_SHOW_GROUPS, OnCheckButtonClicked)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_NOTIFY_OFFLINE, OnCheckButtonClicked)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_NOTIFY_ONLINE, OnCheckButtonClicked)	
		COMMAND_ID_HANDLER_EX(IDC_CHECK_GROUP_BY_ACCOUNTS, OnCheckButtonClicked)	
		COMMAND_ID_HANDLER_EX(IDC_CHECK_TOPMOST, OnClickTopMost)
        
		COMMAND_RANGE_HANDLER_EX(IDC_RADIO_SECURITY_RECEIVE_ALL, IDC_RADIO_SECURITY_RECEIVE_ONLY_FROM_CONTACT_LIST, OnRadioSecurityLevelClicked)
        MESSAGE_HANDLER(CContactNodeLayoutCfg::WM_CONTACT_NODE_LAYOUT_CHANGED, OnContactNodeLayoutChanged)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()
	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnDestroy();
	void OnRadioContactsSortMethodClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnRadioImageSizeClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnClickTopMost(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/);
	void OnRadioSecurityLevelClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);

	HWND GetBtnSizeImage(SizeImage nSizeImage);
    HWND GetBtnSortType(UINT nSortType);
	HWND GetBtnSecurityLevel(UINT nSecurityLevel);
    LRESULT OnContactNodeLayoutChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CContactNodeLayoutCfg m_ContactNodeLayoutCfg;
    IUOLMessengerContactListSettingsPtr		m_pSettings;
	CComboBox	m_cmbContactsSortMethod;
	CButton		m_btnShowOffline;
	CButton		m_btnShowGroups;
	CButton		m_btnNotifyOffline;
	CButton		m_btnNotifyOnline;
	CButton		m_btnTopMost;
	CButton		m_btnGroupByAccounts;

	enum NotifyListener
	{
		NL_SORT_TYPE					= 1,
		NL_SHOW_OFFLINE					= 2,
		NL_SHOW_GROUPS					= 4,
		NL_NOTIFY_TYPE					= 8,
		NL_TOPMOST						= 16,
		NL_SIZEIMAGE					= 32,
		NL_SECURITY_LEVEL				= 64,
        NL_GROUP_BY_ACCOUNTS			= 128,
        NL_CONTACT_NODE_LAYOUT			= 256
	};

	DWORD m_dwToNotify;
	SizeImage m_imageSize;
	UINT	  m_nSecurityLevel;
    UINT      m_nSortType;
    CString	  m_sContactNodeLayout;
};
MAKEAUTOPTR(CContactsPropPage);
