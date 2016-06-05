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


#include "UOLPropPage.h"
#include "../skin/BitmapBackgroundElement.h"
#include "ValidateAccountsDialog.h"

#include <interfaces/IUOLMessengerAccount.h>
#include <interfaces/IUOLMessengerProfileManager.h>


#define		KEY_LEN		100

typedef CAtlList	<IUOLMessengerAccountPtr>			AccountsList;

MAKEAUTOPTR (AccountsList);




class CUOLPropPageImpl : public CUOLPropPage
{
public:
    CUOLPropPageImpl();
    ~CUOLPropPageImpl();

	BEGIN_MSG_MAP(CWizardInitDialog)
        CHAIN_MSG_MAP(CUOLPropPage)
    END_MSG_MAP()

	virtual void SetUOLWizard(CUOLWizard* pWizard);

	virtual BOOL HasNextPage();	
	virtual CString GetNextBtnText();	
	virtual CUOLPropPagePtr GetNextPage(CUOLPropPagePtr pCurrentProp);	
	virtual BOOL HasPreviousPage();	
	virtual CString GetPreviousBtnText();	
	virtual CUOLPropPagePtr GetPreviousPage(CUOLPropPagePtr pCurrentProp); 
	virtual CString GetCancelBtnText();	

	virtual BOOL OnBtnPrevious();	
	virtual BOOL OnBtnNext();
	virtual BOOL OnBtnCancel();
	virtual void OnBtnCustom(CUOLPropPagePtr pCurrentProp, UINT nCmdId);

    void CreateBtnCustom(int x, int width);
	void ShowBtnCustom(BOOL bShow, const CString& strText);

	BOOL ValidateProfileData(IUOLMessengerProfileManagerPtr pProfileManager, CAtlList<IUOLMessengerAccountPtr>* pAccountsList, UOLLib::ISettings* pWizardSettings = NULL);
	
	void Finalize();

	void SetTitle(const CString& strTitle);

protected:
    CUOLPropPagePtr m_pPreviousPage;
	CUOLPropPagePtr m_pNextPage;

	CBitmapBackgroundElementPtr m_pBitmapBackgroundElement;

    CValidateAccountsDialogPtr m_pValidate;

	WTL::CButton m_btnCustom;

	HWND m_hBtnCustom;

	CString m_strTitle;

	CFont m_BoldFont;

private:

	static const CString m_strNextText;
	static const CString m_strPreviousText;
	static const CString m_strCancelText;
};