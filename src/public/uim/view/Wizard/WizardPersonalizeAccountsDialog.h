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
#include "UOLPropPageImpl.h"
#include "../TransparentStatic.h"


class CWizardPersonalizeAccountsDialog : public CUOLPropPageImpl
{
public:
    enum { IDD = IDD_DIALOG_WIZARD_PERSONALIZE_ACCOUNTS };
	DECLARE_ELEMENT_NAME("WizardPersonalizeAccountsDialog");

    CWizardPersonalizeAccountsDialog();
	~CWizardPersonalizeAccountsDialog();

    BEGIN_MSG_MAP(CWizardPersonalizeAccounts)
		MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		MSG_WM_PAINT(OnPaint)
        MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_WIZARD_CHANGE_AVATAR, OnButtonChangeAvatar)   
		CHAIN_MSG_MAP(CUOLPropPageImpl)
    END_MSG_MAP()

	void SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList);
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);	
	CString GetName() const;

	void SetUOLWizard(CUOLWizard* pWizard);

    LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	void OnPaint(HDC hDC);
    BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);   
	CUOLPropPagePtr GetNextPage(CUOLPropPagePtr pCurrentProp);
	
	CString GetCancelBtnText();
	BOOL OnBtnCancel();
	BOOL OnBtnNext();
	BOOL HasNextPage();	

	void OnButtonChangeAvatar(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void SetAccounts();



protected:
	int GetDlgID(){	return IDD_DIALOG_WIZARD_PERSONALIZE_ACCOUNTS; }

private:
	COLORREF			m_bkg;			
	WTL::CBrush			m_brushStatic;

	AccountsList*		m_pAccountsList;

	CString				m_strNickname;

	IUOLMessengerUserIconPtr m_pUserIcon;
    IUOLMessengerImagePtr m_pAvatarDefault;

    IUOLMessengerAccountPtr m_pTmpAccount;

	CTransparentStatic m_staticNickname;		
	CTransparentStatic m_staticPhoto;
};

MAKEAUTOPTR(CWizardPersonalizeAccountsDialog);
