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


#include "stdafx.h"

#include "WizardFinishDialog.h"
#include "../image/Image.h"
#include "../image/ImageBuilder.h"
#include "UOLWizard.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"

#include<interfaces/IUOLMessengerAccountManager.h>


CWizardFinishDialog::CWizardFinishDialog()
{
}

LRESULT CWizardFinishDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	ATLASSERT(NULL != m_pWizard);
	m_pWizard->SetBackground(m_pActiveBackground);
	m_pWizard->ShowBtnNext(FALSE);
	m_pWizard->ShowBtnPrevious(FALSE);
	m_pWizard->SetDefaultButton(IDCANCEL);

	CString strUOLFoneMode;
	strUOLFoneMode = m_pWizard->GetWizardSettings()->GetValue("uol.resouce.appmode_uolfone");

	if(0 == (CUIMApplication::GetApplication()->GetCommonSettings()->GetAppMode()).Compare(strUOLFoneMode) )
	{
		IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();
		AccountsListPtr pAccountsList = new AccountsList();
		ValidateProfileData(pProfileManager, pAccountsList.get());
	}
	return TRUE;
}

void CWizardFinishDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
}

CString CWizardFinishDialog::GetName() const
{
	return "__WizardFinishInstance";
}

void CWizardFinishDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}

CString CWizardFinishDialog::GetCancelBtnText()
{
	CString strBtnText;

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardFinishDialog::OnBtnCancel()
{
	IUOLMessengerAccountManagerPtr pAccountManager = CUIMApplication::GetApplication()->GetAccountManager();

	if (pAccountManager != NULL)
	{
		pAccountManager->SaveAccounts();
	}

	m_pWizard->EndDialog(IDOK);

	return FALSE;
}