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

#include <interfaces/IUOLMessengerAccount.h>

#include "AccountEditConsts.h"
#include "AccountEditPropertyPage.h"

#include "JabberAccountAdvancedPropertyPage.h"
#include "ICQAccountAdvancedPropertyPage.h"


class CAccountEditDialog : public WTL::CPropertySheetImpl<CAccountEditDialog>
{
public:
	CAccountEditDialog(IUOLMessengerAccountPtr pAccount, OperationMode mode);
	virtual ~CAccountEditDialog();
	
protected:
	BEGIN_MSG_MAP(CAccountEditDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		MESSAGE_HANDLER_EX(WM_USER_CHANGE_ADVANCED_PROPPAGE, OnChangeAdvancedPropertyPage)
		MESSAGE_HANDLER_EX(WM_USER_GET_ADVANCED_SETTING_STRING, OnGetAdvancedSettingString)
		MESSAGE_HANDLER_EX(WM_USER_SET_ADVANCED_SETTING_STRING, OnSetAdvancedSettingString)
		MESSAGE_HANDLER_EX(WM_USER_GET_ADVANCED_SETTING_BOOLEAN, OnGetAdvancedSettingBoolean)
		MESSAGE_HANDLER_EX(WM_USER_SET_ADVANCED_SETTING_BOOLEAN, OnSetAdvancedSettingBoolean)
		CHAIN_MSG_MAP(WTL::CPropertySheetImpl<CAccountEditDialog>)
		//REFLECT_NOTIFICATIONS() /* Clebson comments: cannot reflect notificatios here, or else the CPropertySheetImpl shall be not work properly */
	END_MSG_MAP()

	LRESULT OnShowWindow(BOOL bShow, int nStatus);
	LRESULT OnOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	LRESULT OnCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	LRESULT OnChangeAdvancedPropertyPage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetAdvancedSettingString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnSetAdvancedSettingString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetAdvancedSettingBoolean(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnSetAdvancedSettingBoolean(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);


private:
	void AddPropertyPages();

private:
	IUOLMessengerAccountPtr		m_pAccount;
	OperationMode				m_mode;

	CAccountEditPropertyPage			m_accountEditPropPage;
	CJabberAccountAdvancedPropertyPage	m_jabberAccountAdvancedPropPage;
	CICQAccountAdvancedPropertyPage		m_icqAccountAdvancedPropPage;
};