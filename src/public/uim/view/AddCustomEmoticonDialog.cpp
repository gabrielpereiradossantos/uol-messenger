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
#include "AddCustomEmoticonDialog.h"
#include "DialogManager.h"
#include "UIMFileDialog.h"

#include "../controller/UIMApplication.h"
#include "../controller/CustomEmoticon/CustomEmoticonManager.h"
#include "../controller/UIManager.h"
#include "resource.h"


CAddCustomEmoticonDialog::CAddCustomEmoticonDialog() : 
		CMainDialog(IDD_DIALOG_ADD_CUSTOM_EMOTICON),
		m_nOperationModeId(OPERATION_MODE_INVALID),
		m_pEmoticonChangingInfoFinder(NULL)
{
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	if (pSettingsManager2)
	{
		m_pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
				pSettingsManager2->GetEmoticonSettings());
	}
}


CAddCustomEmoticonDialog::~CAddCustomEmoticonDialog(void)
{
    if (IsWindow())
    {
        CDialogManager::GetInstance()->UnregisterDialog(m_hWnd);
        EndDialog(IDCANCEL);
    }
}


void CAddCustomEmoticonDialog::InitializeAddCustomEmoticonInfo(
		IUOLMessengerAccountPtr pAccount, const CString& strAddingEmoticonShortcut, 
		const CString& strAddingEmoticonFileName, OperationModeId nOperationModeId, 
		IEmoticonChangingInfoFinder* pEmoticonChangingInfoFinder)
{
	m_pAccount = pAccount;
	m_strAddingEmoticonShortcut = strAddingEmoticonShortcut;
	m_strAddingEmoticonFileName = strAddingEmoticonFileName;
	m_nOperationModeId = nOperationModeId;
	m_pEmoticonChangingInfoFinder = pEmoticonChangingInfoFinder;
}


void CAddCustomEmoticonDialog::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
    __super::SetParameters(pMapParams, pElementTable);
	
    try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "background_color");
		
        m_cBackgroundColor.SetColor(strColor);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "background_color");
		
		m_cBackgroundColor.SetColor(GetSysColor(COLOR_WINDOWTEXT));
	}
	
	try
	{
		CString relativePath;
		GetParameterValue(relativePath, pMapParams, "openFileButtonImage");
		
		IUOLMessengerUIManager3Ptr pUIManager3 = 
				ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(
						CUIMApplication::GetApplication()->GetUIManager());
		
		IUOLMessengerThemePropertiesPtr pThemeProperties;
		
		if (pUIManager3)
		{
			pThemeProperties = pUIManager3->GetCurrentThemeProperties();
		}
		
		if (pThemeProperties)
		{
			m_strOpenFileButtomImage = pThemeProperties->GetDirPath() + _T("\\") + relativePath;
		}
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "openFileButtonImage");
	}
}


LRESULT CAddCustomEmoticonDialog::OnInitDialog(HWND hwnd, LPARAM lParam)
{
    __super::OnInitDialog(NULL, NULL);

	// Save HWND
	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
	
	if (pUIManager)
	{
		pUIManager->SetAddCustomEmoticonWndHandle(m_hWnd);
	}
	
	DoDataExchange(FALSE);
	
	UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	if (pStringLoader)
	{
		CString strTitleFormat;
		CString strTitleParam;
		
		strTitleFormat = pStringLoader->GetValue(_T("uol.resource.IDS_TITLE_WINDOW_DEFAULT"));
		strTitleParam = pStringLoader->GetValue(_T("uol.resource.IDS_ADD_CUSTOM_EMOTICON_DIALOG_TITLE"));
		
		ATLASSERT(FALSE == strTitleFormat.IsEmpty());
		ATLASSERT(FALSE == strTitleParam.IsEmpty());
		
		CString strTitle;
		strTitle.Format(strTitleFormat, strTitleParam);
		
		//Set title and text
		SetTitle(strTitle);
	}
	
	m_wndEditShortcut = GetDlgItem(IDC_EDIT_SHORTCUT);
	ATLASSERT(m_wndEditShortcut.IsWindow());
	m_wndEditShortcut.LimitText(IUOLMessengerCustomEmoticon::MAX_CUSTOM_EMOTICON_SHORTCUT_LENGTH);
	
	m_shortcutTransparencyApplier.SubclassWindow(GetDlgItem(IDC_STATIC_ATALHO));
	
	::GetClientRect(GetDlgItem(IDC_STATIC_CUSTOM_EMOTICON), &m_emoticonRect);
	m_emoticonRect.right = m_emoticonRect.left + IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_WIDTH;
	m_emoticonRect.bottom = m_emoticonRect.top + IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_HEIGHT;
	
	m_customEmoticonDrawer.SubclassWindow(GetDlgItem(IDC_STATIC_CUSTOM_EMOTICON));
	
	/*
	CRect rectButton(44, 75, 172, 98);
	m_openFileButton.Create(m_hWnd, rectButton, NULL, 
			WS_CHILDWINDOW | BS_OWNERDRAW, 0, ID_BUTTON_OPEN_IMAGE);
	m_openFileMultiStateButton.SubclassWindow(m_openFileButton.m_hWnd);
	*/
	m_openFileButton = GetDlgItem(ID_BUTTON_OPEN_IMAGE);
	
	if ( (m_nOperationModeId == OPERATION_MODE_ADD_FROM_PREFERENCES) || 
			(m_nOperationModeId == OPERATION_MODE_CHANGE_FILEPATH) )
	{
		/*
		m_openFileMultiStateButton.SetMultiStateBitmap(m_strOpenFileButtomImage);
		
		m_openFileButton.SetWindowPos(NULL, rectButton.left, rectButton.top, 
				m_openFileMultiStateButton.GetWidth(), 
				m_openFileMultiStateButton.GetHeight(), SWP_NOZORDER);
		*/
		
		m_openFileButton.ShowWindow(SW_SHOW);
		
		if (m_nOperationModeId == OPERATION_MODE_CHANGE_FILEPATH)
		{
			m_wndEditShortcut.EnableWindow(FALSE);
		}
	}
	else
	{
		m_openFileButton.ShowWindow(SW_HIDE);
	}
	
	if (!m_strAddingEmoticonFileName.IsEmpty())
	{
		if (m_customEmoticonDrawer.Load(m_strAddingEmoticonFileName))
		{
			CSize szImage = m_customEmoticonDrawer.GetSize();
			LONG lImageHeight = szImage.cy;
			LONG lImageWidth = szImage.cx;
			
			CCustomEmoticonManager::CalculateImagePresentationSize(lImageHeight, lImageWidth);
			
			m_emoticonRect.right = m_emoticonRect.left + lImageWidth;
			m_emoticonRect.bottom = m_emoticonRect.top + lImageHeight;
			
			m_customEmoticonDrawer.SetBkColor(m_cBackgroundColor);
			m_customEmoticonDrawer.SetPaintRect(m_emoticonRect);
			m_customEmoticonDrawer.SetStretchImage(TRUE);
			m_customEmoticonDrawer.Draw();
		}
	}
	
	if (!m_strAddingEmoticonShortcut.IsEmpty())
	{
		m_wndEditShortcut.SetWindowText(m_strAddingEmoticonShortcut);
		
		if (m_nOperationModeId != OPERATION_MODE_CHANGE_FILEPATH)
		{
			m_wndEditShortcut.SetSel(0, -1);
		}
	}
	
	if ( (m_nOperationModeId == OPERATION_MODE_CHANGE_SHORTCUT) || 
			(m_nOperationModeId == OPERATION_MODE_CHANGE_FILEPATH) )
	{
		m_pChangingEmoticon = m_pEmoticonSettings3->GetEmoticon(
				IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_ALL, 
				m_strAddingEmoticonShortcut);
	}
	
	m_wndEditShortcut.SetFocus();
	
	CenterWindow();
	
    CDialogManager::GetInstance()->RegisterDialog(m_hWnd);
	
	return 0L;
}


LRESULT CAddCustomEmoticonDialog::OnDestroy()
{
	__super::OnDestroy();
	
	m_customEmoticonDrawer.UnLoad();

	CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(
			CUIMApplication::GetApplication()->GetUIManager());
	
	if (pUIManager)
	{
		pUIManager->SetEmoticonAdditionOnTheWay(FALSE);
		pUIManager->SetAddCustomEmoticonWndHandle(NULL);
	}
	
	return 0L;
}


void CAddCustomEmoticonDialog::OnBnClickedButtonOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	DoDataExchange(TRUE);
	
	if (IsValidData())
	{
		EndDialog(IDOK);
	}
}


void CAddCustomEmoticonDialog::OnBnClickedButtonCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	EndDialog(IDCANCEL);
}


void CAddCustomEmoticonDialog::OnBnClickedOpenImage(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
    CWindow appWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();
	CString strFileName;
	CString strFileExt;
	
	CUIMFileDialog fileDialog(TRUE, NULL, 
			(m_strAddingEmoticonFileName.IsEmpty()) ? NULL : m_strAddingEmoticonFileName.GetBuffer(), 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
			_T("Arquivos de Imagem (*.gif;*.jpg)|*.gif;*.jpg|"), appWindow.m_hWnd);
	
	if ( (IDOK == fileDialog.DoModal()) && (fileDialog.m_szFileName != NULL) )
	{
		strFileName = fileDialog.m_szFileName;
		
		if (m_customEmoticonDrawer.IsPlaying())
		{
			m_customEmoticonDrawer.UnLoad();
		}
		
		if (!strFileName.IsEmpty())
		{
			strFileExt = ATLPath::FindExtension(strFileName);
			strFileExt.Delete(0, 1);
		}
		
		if ( (!m_customEmoticonDrawer.Load(strFileName)) || 
				( (strFileExt.CompareNoCase("gif") != 0) && (strFileExt.CompareNoCase("jpg") != 0) ) )
		{
			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			CString strFormat;
			CString strMessage;
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogWarning(__FUNCTION__, 
					"O arquivo [%s] é inválido. Somente os formatos (*.gif;*.jpg) são aceitos.", 
					strFileName);
			
			strFormat = pStringLoader->GetValue("uol.resource.error.IDS_EMOTICONS_PROPPAGE_INVALID_IMAGE_FILE");
			strMessage.Format(strFormat, strFileName);
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		}
		else
		{
			CSize szImage = m_customEmoticonDrawer.GetSize();
			LONG lImageHeight = szImage.cy;
			LONG lImageWidth = szImage.cx;
			
			CCustomEmoticonManager::CalculateImagePresentationSize(lImageHeight, lImageWidth);
			
			m_emoticonRect.right = m_emoticonRect.left + lImageWidth;
			m_emoticonRect.bottom = m_emoticonRect.top + lImageHeight;
			
			m_customEmoticonDrawer.SetBkColor(m_cBackgroundColor);
			m_customEmoticonDrawer.SetPaintRect(m_emoticonRect);
			m_customEmoticonDrawer.SetStretchImage(TRUE);
			m_customEmoticonDrawer.Draw();
			
			m_strAddingEmoticonFileName = strFileName;
		}
	}
	
	m_wndEditShortcut.SetFocus();
}


INT_PTR CAddCustomEmoticonDialog::ShowModal()
{
	return DoModal();
}


CString CAddCustomEmoticonDialog::GetCustomEmoticonShortcut()
{
	return m_strAddingEmoticonShortcut;
}


CString CAddCustomEmoticonDialog::GetCustomEmoticonFileName()
{
	return m_strAddingEmoticonFileName;
}


BOOL CAddCustomEmoticonDialog::IsValidData()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	
	BOOL bIsValidData = FALSE;
	CString strMsgText;
	BOOL bEmoticonAlreadyInUse = FALSE;
	BOOL bIsShortcutPersisted = FALSE;
	BOOL bIsShortcutScheduledToDeletion = FALSE;
	BOOL bIsShortcutScheduledToChange = FALSE;
	EmoticonFindParams* pFindParams = NULL;
	BOOL bIsShortcutOnPendingOperation = FALSE;
	
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	
	m_strAddingEmoticonShortcut.Trim();
	
	// Test empty shortcut...
	if (m_strAddingEmoticonShortcut.IsEmpty())
	{
		DoDataExchange(FALSE, IDC_EDIT_SHORTCUT);
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CUSTOM_EMOTICON_INVALID_SHORTCUT_ERROR_MSG");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bIsValidData;
	}
	
	if (m_strAddingEmoticonShortcut.Find(" ", 0) > -1)
	{
		DoDataExchange(FALSE, IDC_EDIT_SHORTCUT);
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_EMOTICONS_PROPPAGE_ERROR_SPACE");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bIsValidData;
	}
	
	m_strAddingEmoticonFileName.Trim();
	
	// Test empty shortcut...
	if ( (m_strAddingEmoticonFileName.IsEmpty()) || 
			(!ATLPath::FileExists(m_strAddingEmoticonFileName)) )
	{
		DoDataExchange(FALSE, IDC_EDIT_SHORTCUT);
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CUSTOM_EMOTICON_INVALID_FILE_PATH_ERROR_MSG");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bIsValidData;
	}
	
	// Test shortcut already in use...
	//
	if ( (m_nOperationModeId != OPERATION_MODE_CHANGE_FILEPATH) && (m_pEmoticonSettings3) )
	{
		// Test shortcut against persisted emoticons...
		//
		if (m_pEmoticonSettings3->IsEmoticonShortcut(
					IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_ALL, 
					m_strAddingEmoticonShortcut))
		{
			bIsShortcutPersisted = TRUE;
		}
		
		// Test pending operations considering the shortcut ...
		//
		if (m_pEmoticonChangingInfoFinder != NULL)
		{
			// Considering that we received the Finder pointer and have 
			// it stored on "m_pEmoticonChangingInfoFinder" member.
			// (This window was called from the EmoticonPropPage).
			//
			
			// Is this shortcut being used on a pending deletion operation ?
			//
			if (m_pEmoticonChangingInfoFinder->FindChangingItem(m_strAddingEmoticonShortcut, 
					IEmoticonChangingInfoFinder::OPERATION_ID_DEL, FALSE))
			{
				bIsShortcutScheduledToDeletion = TRUE;
			}
			
			// Is this shortcut being used on a pending addition or shortcut 
			// change operation ?
			//
			if (m_pEmoticonChangingInfoFinder->FindChangingItem(m_strAddingEmoticonShortcut, 
					IEmoticonChangingInfoFinder::OPERATION_ID_ADD | 
					IEmoticonChangingInfoFinder::OPERATION_ID_CHANGE_SHORTCUT, FALSE))
			{
				bIsShortcutScheduledToChange = TRUE;
			}
		}
		else
		{
			// Considering that we didn't receive the Finder pointer.
			// (This window was called from the ConversationWindow).
			//
			HRESULT hResult = E_FAIL;
			HWND hEmoticonPropPageWnd = NULL;
			
			IUOLMessengerUIManager5Ptr pUIManager5 = 
					ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
							CUIMApplication::GetApplication()->GetUIManager());
			
			if (pUIManager5)
			{
				hEmoticonPropPageWnd = pUIManager5->GetEmoticonPropPageWndHandle();
			}
			
			if (hEmoticonPropPageWnd != NULL)
			{
				pFindParams = new EmoticonFindParams;
				
				if (pFindParams)
				{
					// Is this shortcut being used on a pending operation ?
					//
					pFindParams->bConsiderOldShortcut = FALSE;
					pFindParams->nChangingInfoOperations = 
							IEmoticonChangingInfoFinder::OPERATION_ID_ALL;
					pFindParams->strShortcut = m_strAddingEmoticonShortcut;
					pFindParams->bFound = FALSE;
					
					hResult = ::SendMessage(hEmoticonPropPageWnd, 
							IEmoticonChangingInfoFinder::WM_FIND_CHANGING_INFO, 
							0, (LPARAM)pFindParams);
					
					if ( (SUCCEEDED(hResult)) && (pFindParams->bFound) )
					{
						bIsShortcutOnPendingOperation = TRUE;
					}
					
					// Is this shortcut being used on a pending shortcut 
					// change operation as the original shortcut ?
					//
					pFindParams->bConsiderOldShortcut = TRUE;
					pFindParams->nChangingInfoOperations = 
							IEmoticonChangingInfoFinder::OPERATION_ID_CHANGE_SHORTCUT;
					pFindParams->strShortcut = m_strAddingEmoticonShortcut;
					pFindParams->bFound = FALSE;
					
					hResult = ::SendMessage(hEmoticonPropPageWnd, 
							IEmoticonChangingInfoFinder::WM_FIND_CHANGING_INFO, 
							0, (LPARAM)pFindParams);
					
					if ( (SUCCEEDED(hResult)) && (pFindParams->bFound) )
					{
						bIsShortcutOnPendingOperation = TRUE;
					}
					
					/*
					// Is this shortcut being used on a pending deletion operation ?
					//
					pFindParams->nChangingInfoOperations = 
							IEmoticonChangingInfoFinder::OPERATION_ID_DEL;
					pFindParams->strShortcut = m_strAddingEmoticonShortcut;
					pFindParams->bFound = FALSE;
					
					hResult = ::SendMessage(hEmoticonPropPageWnd, 
							IEmoticonChangingInfoFinder::WM_FIND_CHANGING_INFO, 
							0, (LPARAM)pFindParams);
					
					if ( (SUCCEEDED(hResult)) && (pFindParams->bFound) )
					{
						bIsShortcutScheduledToDeletion = TRUE;
					}
					
					// Is this shortcut being used on a pending addition or shortcut 
					// change operation ?
					//
					pFindParams->nChangingInfoOperations = 
							IEmoticonChangingInfoFinder::OPERATION_ID_ADD | 
							IEmoticonChangingInfoFinder::OPERATION_ID_CHANGE_SHORTCUT;
					pFindParams->bFound = FALSE;
					
					hResult = ::SendMessage(hEmoticonPropPageWnd, 
							IEmoticonChangingInfoFinder::WM_FIND_CHANGING_INFO, 
							0, (LPARAM)pFindParams);
					
					if ( (SUCCEEDED(hResult)) && (pFindParams->bFound) )
					{
						bIsShortcutScheduledToChange = TRUE;
					}
					*/
					
					delete pFindParams;
				}
			}
		}
		
		if ( ( (bIsShortcutPersisted) && (!bIsShortcutScheduledToDeletion) ) || 
				(bIsShortcutScheduledToChange) )
		{
			bEmoticonAlreadyInUse = TRUE;
		}
	}
	
	if (bIsShortcutOnPendingOperation)
	{
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CUSTOM_EMOTICON_OPERATION_PENDING_ERROR_MSG");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bIsValidData;
	}
	
	if (bEmoticonAlreadyInUse)
	{
		strMsgText = pStringLoader->GetValue("uol.resource.error.IDS_CUSTOM_EMOTICON_SHORTCUT_FOUND_ERROR_MSG");
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMsgText, MB_OK | MB_ICONERROR);
		
		return bIsValidData;
	}
	
	return TRUE;
}

