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
 
#include "StdAfx.h"
#include ".\profilemanager.h"

#include "../view/wizard/WizardInitDialog.h"
#include "../view/wizard/WizardSplashDialog.h"
#include "../view/wizard/WizardMSNAccountDialog.h"
#include "../view/wizard/UOLWizard.h"
#include "../view/wizard/WizardController.h"

#include "../view/wizard/WizardFinishDialog.h"
#include "UIMApplication.h"
#include "UIManager.h"


CProfileManager::CProfileManager(void) :
m_bCreateAccounts(FALSE)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pAppProfile = new CProfileSettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pAppProfile = [%x].", m_pAppProfile.get());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

CProfileManager::~CProfileManager(void)
{
}

BOOL CProfileManager::LoadSettings()
{
	BOOL bLoadOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	bLoadOk = m_pAppProfile->Load();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bLoadOk = [%d].", bLoadOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bLoadOk;
}

BOOL CProfileManager::SaveSettings()
{
	BOOL bSavedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	bSavedOk = m_pAppProfile->Save();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bSavedOk = [%d].", bSavedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bSavedOk;
}

void CProfileManager::GetProfileList(CAtlList <CString>& profileList)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pAppProfile->GetProfileSettings()->GetProfileList(profileList);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Returning [%d] profiles on list.", profileList.GetCount());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

BOOL CProfileManager::GetProfileElement(const CString& strProfileName, IUOLMessengerProfileElementPtr& pProfileElement)
{
	BOOL bGettingElementOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	bGettingElementOk = m_pAppProfile->GetProfileSettings()->GetProfileElement(strProfileName, pProfileElement);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Getting profile element [%s] returned [%d].", 
			strProfileName, bGettingElementOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bGettingElementOk;
}

BOOL CProfileManager::CreateProfile(const CString& name, IUOLMessengerProfileElementPtr& profileElement)
{
	BOOL bCreateOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (name.FindOneOf(_T("?<>:*\"|\\/")) < 0)
	{
		if (m_pAppProfile->GetProfileSettings()->AddProfile(name, profileElement))
		{
			if (FALSE == ATLPath::FileExists(profileElement->GetProfilePath()))
			{
				::CreateDirectory(profileElement->GetProfilePath(), NULL);
			}

			bCreateOk = m_pAppProfile->Save();
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bCreateOk = [%d].", bCreateOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bCreateOk;
}

BOOL CProfileManager::IsProfile(const CString& name)
{
	BOOL bIsProfile = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	bIsProfile = m_pAppProfile->GetProfileSettings()->IsProfile(name);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Profile name [%s] validation returned [%d].", 
			name, bIsProfile);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bIsProfile;
}

BOOL CProfileManager::RemoveProfile(const CString& name)
{
	BOOL bRemovedOk = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	MAKEAUTOPTR(TCHAR);

	IUOLMessengerProfileElementPtr pProfileElement;
	CString path;

	if (m_pAppProfile->GetProfileSettings()->GetProfileElement(name, pProfileElement))
	{
		path = pProfileElement->GetProfilePath();

		m_pAppProfile->GetProfileSettings()->RemoveProfile(name);
		m_pAppProfile->Save();

		int nBufferSize = path.GetLength() + 2;		

		TCHARPtr pBuffer(new TCHAR[nBufferSize]);
		::ZeroMemory(pBuffer.get(), nBufferSize);

		::_tcsncpy(pBuffer.get(), (LPCTSTR)path, nBufferSize - 1);
	
		SHFILEOPSTRUCT shFileOp;
		::ZeroMemory(&shFileOp, sizeof(SHFILEOPSTRUCT));
		shFileOp.hwnd = NULL;
		shFileOp.wFunc = FO_DELETE;
		shFileOp.pFrom = pBuffer.get();
		shFileOp.pTo = NULL;
		shFileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		
		if (SHFileOperation(&shFileOp) == 0)
		{
			bRemovedOk = TRUE;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRemovedOk = [%d].", bRemovedOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bRemovedOk;
}

void CProfileManager::SetCurrentProfile(const CString& name)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Changing current profile to [%s].", name);
	
	m_pAppProfile->GetProfileSettings()->SetLastSelected(name);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CProfileManager::SetDefaultProfile(const CString& name)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Changing default profile to [%s].", name);
	
	m_pAppProfile->GetProfileSettings()->SetDefaultProfile(name);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

CString CProfileManager::GetCurrentProfile()
{
	CString strCurrentProfile;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	strCurrentProfile = m_pAppProfile->GetProfileSettings()->GetLastSelected();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Getting current profile returned [%s].", strCurrentProfile);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return strCurrentProfile;
}

CString CProfileManager::GetDefaultProfile()
{
	CString strDefaultProfile;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	strDefaultProfile = m_pAppProfile->GetProfileSettings()->GetDefaultProfile();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Getting default profile returned [%s].", strDefaultProfile);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return strDefaultProfile;
}

BOOL CProfileManager::ShowWizardInit()
{
	BOOL bRet = FALSE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
    CString strDefaultProfile;
	CAtlList <CString> profileList;

	GetProfileList(profileList);
	strDefaultProfile = GetDefaultProfile();

	BOOL bOpenDefaultProfile = (FALSE == strDefaultProfile.IsEmpty());

	if (bOpenDefaultProfile)
	{
		SetCurrentProfile(strDefaultProfile);
	}
	
	CRect rctDimWindow(0, 0, 558, 390);
	CUOLWizard dlgWiz(IDD_DIALOG_WIZARD);
	dlgWiz.SetInitialRect(rctDimWindow);
	dlgWiz.SetCenterWindow(TRUE);
	
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	
	if (profileList.GetCount() > 0)
	{
		CWizardSplashDialogPtr pWizardSplash = ap_dynamic_cast<CWizardSplashDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardSplashInstance"));
		dlgWiz.SetPage(pWizardSplash);

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->SetProgressWindowVisible(FALSE);
		int nRet = dlgWiz.DoModal(NULL);
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->SetProgressWindowVisible(TRUE);

		if (nRet == ID_CREATEPROFILE)
		{
            m_bCreateAccounts = TRUE;
		}

        bRet = ((nRet != IDCANCEL) ? TRUE : FALSE);
	}
	else
	{
		CWizardInitDialogPtr pWizardInit = ap_dynamic_cast<CWizardInitDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardInitInstance"));
		dlgWiz.SetPage(pWizardInit);

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->SetProgressWindowVisible(FALSE);
		int nRet = dlgWiz.DoModal(NULL);
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->SetProgressWindowVisible(TRUE);

		if (nRet == ID_CREATEPROFILE)
		{
            m_bCreateAccounts = TRUE;

			bRet = TRUE;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d].", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bRet;
}

BOOL CProfileManager::ShowWizardAccounts()
{
	BOOL bRet = TRUE;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (m_bCreateAccounts)
	{
		CRect rctDimWindow(0, 0, 558, 390);
		CUOLWizard dlgWiz(IDD_DIALOG_WIZARD);
		dlgWiz.SetInitialRect(rctDimWindow);
		dlgWiz.SetCenterWindow(TRUE);

		IUOLMessengerFacade2* pUIMApp = CUIMApplication::GetApplication();
		CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(pUIMApp->GetUIManager());

		if(0 == (pUIMApp->GetCommonSettings()->GetAppMode()).Compare(IUOLMessengerCommonSettings::APP_MODE_DEFAULT_VALUE) )
		{
			dlgWiz.SetPage(CWizardController::GetInstance()->GetNextValidPage(ID_PROFILE_WIZARD_PAGE));
		}
		else //Is UOLFone
		{
			CWizardFinishDialogPtr pWizardFinish = ap_dynamic_cast<CWizardFinishDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardFinishInstance"));
			dlgWiz.SetPage(pWizardFinish );
		}	

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->SetProgressWindowVisible(FALSE);
		int nRet = dlgWiz.DoModal(NULL);
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->SetProgressWindowVisible(TRUE);

		bRet = (nRet == IDOK ? TRUE : FALSE);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d].", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bRet;
}
