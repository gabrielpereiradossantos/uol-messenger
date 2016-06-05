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
#include "FileTransferWindow.h"
#include "../UOLToolbarBuilder.h"
#include "../ImageButton.h"
#include "../../resource.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/uimanager.h"


#define MIN_WIDTH 300

///////////////////////////////////////////////////////////////////////////////
// CFileTransferWindow Class
CFileTransferWindow::CFileTransferWindow(CMainFrame* pParentFrame) :
	m_pParentFrame(pParentFrame)
{
	ATLASSERT(m_pParentFrame != NULL);

	// Create File Transfer List
	m_pFileTransferList = new CFileTransferList();
	m_pToolbar = new CUOLToolBar();
	
	IUOLMessengerSettingsManagerPtr pSettingsManager = 
			CUIMApplication::GetApplication()->GetSettingsManager();
	
	if (pSettingsManager)
	{
		m_pMessageSettings3 = ap_static_cast<IUOLMessengerMessageSettings3Ptr>(
				pSettingsManager->GetMessageSettings());
	}
	
	if (m_pMessageSettings3)
	{
		m_pMessageSettings3->AddEventListener(this);
	}
}


CFileTransferWindow::~CFileTransferWindow()
{
	if (m_pMessageSettings3)
	{
		m_pMessageSettings3->RemoveEventListener(this);
	}
	
	if (m_pFileTransferList)
	{
		m_pFileTransferList.reset();
	}
}


LRESULT CFileTransferWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	// Set window title
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strTitle = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CAPTION");
	m_pParentFrame->SetCaptionText(strTitle);

	// Create FileTransferListCtrl
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	m_pFileTransferListCtrl = ap_dynamic_cast<CFileTransferListCtrlPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__FileTransferListInstance"));

	if (m_pFileTransferListCtrl != NULL)
	{
		IUOLMessengerFacade3* pApp = CUIMApplication::GetApplication();
		m_pFileTransferListCtrl->Init(m_pFileTransferList);
		
		CRect rect = CRect(0, 0, 0, 0);
		m_pFileTransferListCtrl->Create(m_hWnd, rect, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}

	CreateToolbar();

	return 0L;
}


void CFileTransferWindow::OnDestroy()
{
	if (m_pFileTransferListCtrl.get())
	{
		m_pFileTransferListCtrl->Finalize();
	}

	SetMsgHandled(FALSE);
}


void CFileTransferWindow::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);
    CSize size(rect.Width(), rect.Height());

    // Toolbar control
	CSize sizeToolbar;
    if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);
    }

    if (m_pToolbar.get())
	{
		m_pToolbar->SetWindowPos(NULL, 0, 0, size.cx, sizeToolbar.cy, SWP_NOZORDER);
	}

	// FileTransferList control
    int x = rect.left;
    int y = rect.top + sizeToolbar.cy;
    int cx = size.cx;
    int cy = size.cy - sizeToolbar.cy;

    if (m_pFileTransferListCtrl)
    {
        m_pFileTransferListCtrl->SetWindowPos(NULL, x, y, cx, cy, SWP_NOZORDER);
    }
}


void CFileTransferWindow::GetMinSize(CSize& size)
{
	CSize sizeFileTransfer(0, 0);
	if (m_pFileTransferListCtrl.get())
	{
		m_pFileTransferListCtrl->GetMinSize(sizeFileTransfer);
	}

	CSize sizeToolbar(0, 0);
	if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);
	}

	int cy = sizeFileTransfer.cy +  sizeToolbar.cy;
	size.SetSize(MIN_WIDTH, cy);
}


void CFileTransferWindow::CreateToolbar()
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	CUOLToolBarBuilderPtr pBuilder;

	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pBuilder.get() == NULL)); )
	{
		pBuilder = ap_dynamic_cast<CUOLToolBarBuilderPtr>(m_listChilds.GetNext(pos));
	}	

	if (pBuilder)
	{
		m_pToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pToolbar);
	}

	if (m_pToolbar.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pToolbar->Create(m_hWnd, rect);			
		ATLASSERT(m_pToolbar.get());
		m_pToolbar->AddButtonsEventListener(this);
		
		UpdateToolbarButtons();
	}
}


void CFileTransferWindow::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strConfirmMessage;
	
	CString cmdName = pButton->GetCommandName();

	if (cmdName.CompareNoCase(_T("CancelAll")) == 0)
    {
		strConfirmMessage = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CANCEL_ALL_CONFIRMATION");
		
		UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, 
			strConfirmMessage, MB_YESNO | MB_ICONQUESTION);
		if (ret == IDYES)
		{
			m_pFileTransferListCtrl->CancelAllFileTransfers();
		}		
    }
	else if (cmdName.CompareNoCase(_T("DeleteAll")) == 0)
    {
		strConfirmMessage = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CLEAR_ALL_CONFIRMATION");
		
		UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, 
			strConfirmMessage, MB_YESNO | MB_ICONQUESTION);
		if (ret == IDYES)
		{
			m_pFileTransferListCtrl->RemoveAllFinishedFileTransfers();
		}
    }
	else if (cmdName.CompareNoCase(_T("OpenFolder")) == 0)
    {
		OpenFolder();
    }
}


void CFileTransferWindow::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
	if (nSettingId == IUOLMessengerMessageSettings::MSG_FILE_TRANSFER_SAVE_FOLDER_MODE)
	{
		UpdateToolbarButtons();
	}
}


void CFileTransferWindow::OpenFolder()
{
	CString strFolderPath;

	IUOLMessengerSettingsManagerPtr	pSettingManager;
	pSettingManager = CUIMApplication::GetApplication()->GetSettingsManager();
	
	// Get last received file folder
	IUOLMessengerGeneralSettingsPtr pGeneralSettings;
	pGeneralSettings = pSettingManager->GetGeneralSettings();
	if (pGeneralSettings)
	{
		strFolderPath = pGeneralSettings->GetLastReceiveFileFolder();
	}
	
	// Get file transfer save folder mode from settings
	IUOLMessengerMessageSettings3Ptr pMessageSettings3;
	pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(pSettingManager->GetMessageSettings());
	if (pMessageSettings3)
	{
		DWORD nFileTransferSaveFolderMode = pMessageSettings3->GetFileTransferSaveFolderMode();
		
		if (nFileTransferSaveFolderMode == IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT)
		{
			strFolderPath = pMessageSettings3->GetDefaultFileTransferFolder();
		}
	}

	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = (LPCTSTR) _T("explore");
	shExecInfo.lpFile = (LPCTSTR) strFolderPath;
	shExecInfo.lpParameters = NULL;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOWNORMAL;
	shExecInfo.hInstApp = NULL;

	BOOL bExecute = ::ShellExecuteEx(&shExecInfo);
	if (!bExecute)
	{
		DWORD dwError = ::GetLastError();
		ATLTRACE(_T("Error %d: couldn't open folder %s\n"), dwError, (LPCTSTR) strFolderPath);
	}
}


void CFileTransferWindow::UpdateToolbarButtons()
{
	if ( (!m_pToolbar) || (!m_pMessageSettings3) )
	{
		return;
	}
	
	IUOLMessengerImageButtonPtr pImageButton = 
			m_pToolbar->FindImageButton("OpenFolder");
	
	if (!pImageButton)
	{
		return;
	}
	
	if (m_pMessageSettings3->GetFileTransferSaveFolderMode() == 
			IUOLMessengerMessageSettings3::FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT)
	{
		pImageButton->SetVisible(TRUE);
	}
	else
	{
		pImageButton->SetVisible(FALSE);
	}
}

