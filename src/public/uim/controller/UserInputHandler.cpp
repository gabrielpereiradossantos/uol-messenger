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
         

#include "UserInputHandler.h"

#include "../resource.h"
#include "UIMApplication.h"
#include "UIManager.h"
#include "../view/AuthorizeDialog.h"
#include "../view/InputDialog.h"
#include "../view/ConfirmDlg.h"
#include "../view/UIMFileDialog.h"

#include <interfaces/IUOLMessengerSettingsManager.h>
#include <interfaces/IUOLMessengerGeneralSettings.h>


CUserInputHandler::CUserInputHandler()
{
}


CUserInputHandler::~CUserInputHandler()
{
}


RequestResult CUserInputHandler::RequestInput(IUOLMessengerAccountPtr pAccount, RequestReason reason, CString& strUserResponse, int nResponseLength)
{
	RequestResult result = REQUEST_RESULT_CANCEL;
	
	if (reason == REQUEST_REASON_AUTHORIZE_MESSAGE)
	{
		// Ignore authorize message
		//CString strTitle;
		//CString strLabel;
		
		//strTitle.LoadString(IDS_USER_INPUT_HANDLER_AUTHORIZE_CAPTION);
		//strLabel.LoadString(IDS_USER_INPUT_HANDLER_AUTHORIZE_MESSAGE);
		
		//if (CUIMApplication::GetApplication()->GetUIManager()->ShowInputDlg(strTitle, strLabel, strUserResponse, nResponseLength, FALSE))
		//{
			//result = REQUEST_RESULT_OK; 
		//}
		
		result = REQUEST_RESULT_OK;
	}
	//else if (reason == REQUEST_REASON_DENY_MESSAGE)
	//{
	//}
	//else if (reason == REQUEST_REASON_ENTER_PASSWORD)
	//{
	//}
	
	return result;
}


RequestResult CUserInputHandler::RequestAddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	RequestResult result = REQUEST_RESULT_NONE;

	IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
    ATLASSERT(pUIManager);

    CAuthorizeDialogPtr pAuthorizeDialog = pUIManager->GetAuthorizeDialog();
	pAuthorizeDialog->Initialize(pAccount, strContact, (pContact != NULL));

	if (IDOK == pAuthorizeDialog->DoModal())
	{
		if (pAuthorizeDialog->IsContactAuthorized())
		{
			result = REQUEST_RESULT_OK;
		}
		else
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	return result;
}


RequestResult CUserInputHandler::RequestAddUser(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strAddReason)
{
	return RequestAddContact(pAccount, strContact);
}


RequestResult CUserInputHandler::RequestSendAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact)
{
	return REQUEST_RESULT_OK;
}


RequestResult CUserInputHandler::RequestAcceptFile(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strFilename)
{
	RequestResult result = REQUEST_RESULT_CANCEL;

	// Check if user can receive files from any user
	IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	if ((!pContact) &&
		(IUOLMessengerContactListSettings::SL_RECEIVE_ONLY_FROM_LIST == CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->GetSecurityLevel()))
	{
		return result;
	}

	// Display confirmation dialog
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strCaption;
	CString strMessage;

	strCaption = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
	strMessage.Format(IDS_USER_INPUT_HANDLER_RECV_FILE_ACCEPT, strContact, strFilename);

	BOOL bBadFileType = FALSE;

	IUOLMessengerFileTransferFilterPtr pFileTransferFilter = CUIMApplication::GetApplication()->GetFileTransferManager()->GetFileTransferFilter();

	if (pFileTransferFilter)
	{
		bBadFileType = pFileTransferFilter->IsBadFileType(strFilename);
	}

	UINT nType = MB_YESNO | MB_TOPMOST | MB_APPLMODAL;

	if (bBadFileType)
	{
		nType |= MB_ICONWARNING;

		CString strWarning;
		strWarning.LoadString(IDS_USER_INPUT_HANDLER_RECV_FILE_WARNING);
		strMessage += strWarning;
	}
	else
	{
		nType |= MB_ICONQUESTION;		
	}

	UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, nType);
    if (ret == IDYES)
	{
		result = REQUEST_RESULT_OK;
	}

	return result;
}


RequestResult CUserInputHandler::RequestFilePath(IUOLMessengerAccountPtr pAccount, const CString& strContact, CString& strFilePath, int nMaxPathLength)
{
	IUOLMessengerSettingsManagerPtr	pSettingManager;
	pSettingManager = CUIMApplication::GetApplication()->GetSettingsManager();
	
	CString strFolder;
	BOOL bAskFolder = TRUE;
	RequestResult result = REQUEST_RESULT_CANCEL;

	// Get file transfer save folder mode from settings
	IUOLMessengerMessageSettings3Ptr pMessageSettings3;
	pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(pSettingManager->GetMessageSettings());
	if (pMessageSettings3)
	{
		DWORD nFileTransferSaveFolderMode = pMessageSettings3->GetFileTransferSaveFolderMode();
		
		switch (nFileTransferSaveFolderMode)
		{
		case IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK:
			bAskFolder = TRUE;
			break;

		case IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT:
			strFolder = pMessageSettings3->GetDefaultFileTransferFolder();
			strFolder.Append("\\");
			strFilePath.Insert(0, strFolder);
			bAskFolder = (strFilePath.IsEmpty() ? TRUE : FALSE);
			AvoidOverridingFilePath(strFilePath);
			result = REQUEST_RESULT_OK;
			break;

		default:
			bAskFolder = TRUE;
		}
	}
	
	if (bAskFolder)
	{
		TCHAR szFilter[MAX_PATH + 1] = "Todos os Arquivos (*.*)|*.*|";
		CWindow tmpWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();
		CUIMFileDialog fileDialog(FALSE, NULL, strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, tmpWindow.m_hWnd);
		CString strLastFolder = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetLastReceiveFileFolder();

		fileDialog.m_ofn.lpstrInitialDir = strLastFolder;
		if ((IDOK == fileDialog.DoModal()) && (fileDialog.m_szFileName != NULL)) 
		{
			strFilePath = fileDialog.m_szFileName;

			int nFind = strFilePath.ReverseFind('\\');
			strLastFolder = strFilePath.Left(nFind);

			CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetLastReceiveFileFolder(strLastFolder);

			result = REQUEST_RESULT_OK;
		}
		else
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	return result;
}


RequestResult CUserInputHandler::RequestPassword(IUOLMessengerAccountPtr pAccount, CString& strPassword, int nMaxPasswordLength, BOOL& bSave)
{
	CString strTitle;
	CString strLabel;

	strTitle.Format(_T("%s - %s"), pAccount->GetProtocolName(), pAccount->GetUserId());
	strLabel.LoadString(IDS_ACCOUNT_REQUEST_PASSWORD_DIALOG_LABEL);
		
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
    ATLASSERT(pUIManager);

    CInputDialogPtr pDialogPassword = pUIManager->GetInputDialog();
	pDialogPassword->Initialize(strTitle, strLabel, nMaxPasswordLength, TRUE);

	pDialogPassword->SetText(strPassword);
	pDialogPassword->SetSavePassword(pAccount->IsSavePasswordEnabled());

	int nRet = pDialogPassword->DoModal();

	if (nRet == IDOK)
	{
		strPassword = pDialogPassword->GetText();
		bSave = pDialogPassword->IsSavePassword();

		return REQUEST_RESULT_OK;
	}

	return REQUEST_RESULT_CANCEL;
}


RequestResult CUserInputHandler::RequestUserAuthorization(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage, BOOL& bAddToContactList, CString& strAddGroup)
{
	RequestResult result = REQUEST_RESULT_NONE;

	IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
    ATLASSERT(pUIManager);

    CAuthorizeDialogPtr pAuthorizeDialog = pUIManager->GetAuthorizeDialog();
	pAuthorizeDialog->Initialize(pAccount, strContact, (pContact != NULL));
	
	if (IDOK == pAuthorizeDialog->DoModal())
	{
		if (pAuthorizeDialog->IsContactAuthorized())
		{
			result = REQUEST_RESULT_OK;
			
			bAddToContactList = pAuthorizeDialog->GetAddContactFlag();
			strAddGroup = pAuthorizeDialog->GetContactGroup();
		}
		else
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	return result;
}


RequestResult CUserInputHandler::RequestAddContact(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage, BOOL& bAddToContactList, BOOL bAlreadyOnList, CString& strAddGroup)
{
	RequestResult result = REQUEST_RESULT_NONE;
	BOOL bAutoAddContact = FALSE;
	
	IUOLMessengerContactPtr pContact = CUIMApplication::GetApplication()->GetContactList()->FindContact(pAccount->GetUserId(), pAccount->GetProtocolId(), strContact);
	
	if (pAccount->GetProtocol())
	{
		bAutoAddContact = pAccount->GetProtocol()->AutoAddContactOnAuthorization();
	}
	
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
    ATLASSERT(pUIManager);

    CAuthorizeDialogPtr pAuthorizeDialog = pUIManager->GetAuthorizeDialog();
	pAuthorizeDialog->Initialize(pAccount, strContact, ( (pContact != NULL) || (bAutoAddContact) ) );
	
	if (IDOK == pAuthorizeDialog->DoModal())
	{
		if (pAuthorizeDialog->IsContactAuthorized())
		{
			result = REQUEST_RESULT_OK;
			
			bAddToContactList = pAuthorizeDialog->GetAddContactFlag();
			strAddGroup = pAuthorizeDialog->GetContactGroup();
		}
		else
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}
	
	return result;
}


void CUserInputHandler::AvoidOverridingFilePath(CString& strFilePath)
{
	CString strFinalFilePath = strFilePath;
	BOOL bGeneratedNewName = FALSE;
	
	if (ATLPath::FileExists(strFilePath))
	{
		CString strExtension = strFilePath;
		CString strFullFileNameNoExt = strFilePath;
		
		bGeneratedNewName = TRUE;
		
		// Extracts the extension.
		//
		strExtension.MakeLower();
		
		int nFind = strExtension.ReverseFind('.');
		
		if (nFind != -1)
		{
			strExtension = strExtension.Mid(nFind + 1);
		}
		else
		{
			strExtension = "";
		}
		
		ATLPath::RemoveExtension(strFullFileNameNoExt.GetBuffer());
		
		int iFileIndex = 1;
		
		while (ATLPath::FileExists(strFinalFilePath))
		{
			strFinalFilePath.Format("%s(%d).%s", 
					strFullFileNameNoExt, iFileIndex, strExtension);
			
			iFileIndex++;
		}
	}
	
	if (bGeneratedNewName)
	{
		strFilePath = strFinalFilePath;
	}
}

