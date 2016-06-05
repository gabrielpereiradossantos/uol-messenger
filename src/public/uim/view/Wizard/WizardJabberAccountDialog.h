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

#include <interfaces/IUOLMessengerProfileManager.h>
#include <interfaces/IUOLMessengerAccount.h>

#include "UOLPropPageImpl.h"
#include "../transparentstatic.h"



#define MAX_LEN_PORT		5
#define MAX_PORT		65535



class CWizardJabberAccountDialog : public CUOLPropPageImpl,
    public CWinDataExchange<CWizardJabberAccountDialog>
{
public:
    enum { IDD = IDD_DIALOG_WIZARD_JABBER_ACCOUNT };
	DECLARE_ELEMENT_NAME("WizardJabberAccountDialog");

	CWizardJabberAccountDialog();
	~CWizardJabberAccountDialog();

    BEGIN_MSG_MAP(CWizardJabberAccountDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CTLCOLORSTATIC(OnCtlColor) 
		COMMAND_HANDLER_EX(IDC_BUTTON_WIZARD_JABBER_ADD_ACCOUNT, BN_CLICKED, OnClickButton)
		COMMAND_HANDLER_EX(IDC_LIST_WIZARD_JABBER_ACCOUNTS, LBN_SELCHANGE, OnSelectChange)
		COMMAND_HANDLER_EX(IDC_EDIT_WIZARD_JABBER_EMAIL, EN_KILLFOCUS, OnKillFocus)
        CHAIN_MSG_MAP(CUOLPropPageImpl)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CWizardJabberAccountDialog)
		DDX_TEXT(IDC_EDIT_WIZARD_JABBER_EMAIL, m_strEmail)
		DDX_TEXT(IDC_EDIT_WIZARD_JABBER_PASSWORD, m_strPassword)
		DDX_TEXT(IDC_EDIT_WIZARD_JABBER_SERVER, m_strServer)
		DDX_TEXT(IDC_EDIT_WIZARD_JABBER_PORT, m_strPort)
    END_DDX_MAP()

	void SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList);
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);	
	CString GetName() const;

	void SetUOLWizard(CUOLWizard* pWizard);

    BOOL OnInitDialog (HWND hwndFocus, LPARAM lParam);	
    LRESULT OnCtlColor(HDC hDC, HWND hwnd);

	CString GetCancelBtnText();
	BOOL OnBtnNext();
	BOOL HasNextPage();
	BOOL OnBtnCancel();

	BOOL CaptureJabberAccounts();

	CUOLPropPagePtr GetNextPage(CUOLPropPagePtr pCurrentProp);

    void OnClickButton(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
    void OnSelectChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
    void OnKillFocus(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);

    BOOL CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount);
	BOOL ValidateData(BOOL bIsNextButton);

protected:
	int GetDlgID(){	return IDD_DIALOG_WIZARD_JABBER_ACCOUNT; }

private:
	void LoadConnectServer(const CString& strEmail);

private:
	static const CString m_strIdentitiesKey;
	static const CString m_strProtocolId;
	static const CString m_strDefaultPort;

	CListBox m_lstbCaptured;

    IUOLMessengerAccountPtr m_pAccount;
    AccountsList* m_pAccountsList;

	COLORREF			m_bkg;			
	WTL::CBrush			m_brushStatic;

	CIMProtocol* m_pProtocol;

	CString m_strEmail;
	CString m_strPassword;
	CString m_strServer;
	CString m_strPort;

	CTransparentStatic m_staticEmail;		
	CTransparentStatic m_staticPass;
	CTransparentStatic m_staticPort;		
	CTransparentStatic m_staticServer;
	CTransparentStatic m_staticInfo;
	CTransparentStatic m_staticSubHeader;
};

MAKEAUTOPTR(CWizardJabberAccountDialog);