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

#include "UOLWizard.h"
#include "../image/Image.h"
#include "../image/imagebuilder.h"
#include "WizardPersonalizeAccountsDialog.h"
#include "MinDependencySpecialFolder.h"
#include "WizardFinishDialog.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"




CWizardPersonalizeAccountsDialog::CWizardPersonalizeAccountsDialog() :
m_bkg(RGB(255, 255, 255))
{
	m_pNextPage = NULL;

    m_brushStatic.CreateSolidBrush(m_bkg);

    m_pAccountsList = NULL;
}

CWizardPersonalizeAccountsDialog::~CWizardPersonalizeAccountsDialog()
{
}

void CWizardPersonalizeAccountsDialog::SetAccountsList(CAtlList<IUOLMessengerAccountPtr>* pAccountsList)
{
	m_pAccountsList = pAccountsList;
}

void CWizardPersonalizeAccountsDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	CString strDefaultAvatar;
	GetParameterValue(strDefaultAvatar, pMapParams, _T("defaultusericon"));

	m_pAvatarDefault = UOLMessengerImage::CImageBuilder::LoadImage(strDefaultAvatar);
	ATLASSERT(m_pAvatarDefault.get());
}

CString CWizardPersonalizeAccountsDialog::GetName() const
{
	return "__WizardPersonalizeInstance";
}

void CWizardPersonalizeAccountsDialog::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);
}

BOOL CWizardPersonalizeAccountsDialog::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	/*Init*/
	m_pTmpAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(_T(""), "prpl-msn");
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	CWizardFinishDialogPtr pWizardFinish = ap_dynamic_cast<CWizardFinishDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardFinishInstance"));
	m_pNextPage = pWizardFinish;
	ATLASSERT(NULL!= m_pWizard);
	m_pWizard->SetBackground(m_pActiveBackground);
	/**/

	m_staticNickname.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PERSONALIZE_NICK));		
	m_staticNickname.SetColor(RGB(0, 0, 0));

	m_staticPhoto.SubclassWindow(GetDlgItem(IDC_STATIC_WIZARD_PERSONALIZE_PHOTO));		
	m_staticPhoto.SetColor(RGB(0, 0, 0));

	return TRUE;
}

LRESULT CWizardPersonalizeAccountsDialog::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}

CUOLPropPagePtr CWizardPersonalizeAccountsDialog::GetNextPage(CUOLPropPagePtr pCurrentProp) 
{
    return m_pNextPage;
}

CString CWizardPersonalizeAccountsDialog::GetCancelBtnText()
{
	CString strBtnText;

	ATLASSERT(m_pWizard->GetWizardSettings() != NULL);

	strBtnText = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_WIZARD_FINISH_BTNCANCEL");

	return strBtnText;
}

BOOL CWizardPersonalizeAccountsDialog::OnBtnCancel()
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

BOOL CWizardPersonalizeAccountsDialog::OnBtnNext()
{
	(GetDlgItem(IDC_EDIT_WIZARD_PERSONALIZE_NICK)).GetWindowText(m_strNickname);
    
	SetAccounts();

    return TRUE;
}

BOOL CWizardPersonalizeAccountsDialog::HasNextPage()	
{
	return TRUE;
}

void CWizardPersonalizeAccountsDialog::OnButtonChangeAvatar(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	int nCurSel = -1;

	CMinDependencySpecialFolder mdsf;
	CString strUserDir = mdsf.GetPersonal();

	TCHAR szFilter[MAX_PATH + 1] = "Image Files (*.bmp;*.gif;*.jpeg;*.jpg)\0*.bmp;*.gif;*.jpeg;*.jpg\0\0";

	CFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, m_hWnd);
	fileDialog.m_ofn.lpstrInitialDir = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetLastAvatarFolder();

	if ((IDOK == fileDialog.DoModal()) && (fileDialog.m_szFileName != NULL))
	{
		CString strFilePath = fileDialog.m_szFileName;

		m_pUserIcon = CUIMApplication::GetApplication()->GetUserIconCacheManager()->CacheIcon(m_pTmpAccount.get(), strFilePath);

		m_pAvatarDefault = m_pUserIcon->GetImage();
	}
}

void CWizardPersonalizeAccountsDialog::OnPaint(HDC hDC)
{
	CPaintDC dc(m_hWnd);

	if (m_pAvatarDefault != NULL)
	{
		m_pAvatarDefault->Draw(CDCHandle(dc), 
			245, 145, 
			96, 96,
			new UOLMessengerImage::CDrawStretchOpaqueStrategy());
	}
	SetMsgHandled(FALSE);
}

void CWizardPersonalizeAccountsDialog::SetAccounts()
{
    if(m_pAccountsList) {
	IUOLMessengerAccountPtr pAccount;
	CString strAux = m_strNickname;

	strAux.Trim();

    for (POSITION pos = m_pAccountsList->GetHeadPosition(); pos; m_pAccountsList->GetNext(pos))
	{
        pAccount = m_pAccountsList->GetAt(pos);

		if (m_pUserIcon)
		{
            pAccount->SetUserIcon(m_pUserIcon);
		}

		if (strAux.IsEmpty())
		{
			m_strNickname = pAccount->GetUserId();
		}
        
		pAccount->SetNickName(m_strNickname);
	}
    }
}