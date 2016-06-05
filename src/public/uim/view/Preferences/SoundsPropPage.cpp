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
 
// ContactPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "SoundsPropPage.h"

#include "../../controller/UIMApplication.h"
#include "../UIMFileDialog.h"


typedef struct _SOUNDINFO
{
	IUOLMessengerSoundEventPtr pSoundEvent;
	CString strFullPath;
	BOOL bDirty;
} SOUNDINFO, * LPSOUNDINFO;


struct SoundEventHelper
{
	IUOLMessengerSoundEventPtr pSoundEvent;
};


CSoundsPropPage::CSoundsPropPage() :
	CPreferencesPropPage(IDD_DIALOG_SOUNDS),
	m_dwToNotify(0)
{
	m_strColumnEventType.LoadString(IDS_SOUNDS_PROPPAGE_COLUMN_EVENT_TYPE);
	m_strColumnFilePath.LoadString(IDS_SOUNDS_PROPPAGE_COLUMN_FILE_PATH);
}

CSoundsPropPage::~CSoundsPropPage()
{
}


LRESULT CSoundsPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());

	if (pSettingsManager2)
	{
		m_pSettings = ap_dynamic_cast<CSoundSettingsPtr>(pSettingsManager2->GetSoundSettings());
	}

	ATLASSERT(m_pSettings);

	m_pGeneralSettings = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings();

	m_btnSound = GetDlgItem(IDC_CHECK_SOUND);
	m_btnMuteWhileAway = GetDlgItem(IDC_CHECK_MUTE_WHILE_AWAY);
	m_btnChangeSound = GetDlgItem(IDC_BUTTON_CHANGE_SOUND);
	m_btnPlaySound = GetDlgItem(IDC_BUTTON_PLAY_SOUND);
	m_wndListSounds = GetDlgItem(IDC_LIST_SOUNDS);
	
	m_btnMuteWhileAway.SetCheck(m_pGeneralSettings->IsMuteWhileAwayEnabled());

	if (m_pGeneralSettings->IsEnableSound())
	{
		m_btnSound.SetCheck(true);
		m_btnMuteWhileAway.EnableWindow(true);
	}
	else
	{
		m_btnMuteWhileAway.EnableWindow(false);
		m_btnSound.SetCheck(false);
	}

	// Add sound list column headers
	m_wndListSounds.SetExtendedListViewStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_wndListSounds.InsertColumn(0, m_strColumnEventType, LVCFMT_LEFT, 258, -1);
	m_wndListSounds.InsertColumn(1, m_strColumnFilePath, LVCFMT_LEFT, 200, -1);

	UpdateSoundEventList();

	//m_btnChangeSound.EnableWindow(FALSE);

	m_dwToNotify = 0;

	return 0L;
}


void CSoundsPropPage::OnDestroy()
{
	// Release sound event info memory
	int nCount = m_wndListSounds.GetItemCount();

	for (int nInd = 0; nInd < nCount; nInd++)
	{
		LPSOUNDINFO pSoundInfo = (LPSOUNDINFO) m_wndListSounds.GetItemData(nInd);

		if (pSoundInfo)
		{
			m_wndListSounds.SetItemData(nInd, NULL);
			delete pSoundInfo;
		}
	}
}


LRESULT CSoundsPropPage::OnShowWindow(BOOL bShow, int nStatus)
{
	if (bShow)
	{
		UpdateSoundEventList();
	}
	
	return 0L;
}


void CSoundsPropPage::OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	switch (nID)
	{
        case IDC_CHECK_SOUND:

            if (m_btnSound.GetCheck() == BST_CHECKED)
            {
                //m_pGeneralSettings->EnableSound(true);
                m_btnMuteWhileAway.EnableWindow(true);
            }
            else
            {
                //m_pGeneralSettings->EnableSound(false);
                m_btnMuteWhileAway.EnableWindow(false);
            }
			m_dwToNotify |= NL_MUTE;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
			//m_pSettings->NotifyEventListeners(IUOLMessengerGeneralSettings::SD_MUTE);

        break;

        case IDC_CHECK_MUTE_WHILE_AWAY:

            //m_pGeneralSettings->EnableMuteWhileAway(m_btnMuteWhileAway.GetCheck() == BST_CHECKED ? TRUE : FALSE);
			m_dwToNotify |= NL_MUTE_WHILE_AWAY;
			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
			//m_pSettings->NotifyEventListeners(IUOLMessengerGeneralSettings::SD_MUTE_WHILE_AWAY);

        break;
	}
}

void CSoundsPropPage::OnButtonClickPlaySound(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	int nSelectedIndex = m_wndListSounds.GetSelectedIndex();

	if (nSelectedIndex >= 0)
	{
		LPSOUNDINFO pSoundInfo = (LPSOUNDINFO) m_wndListSounds.GetItemData(nSelectedIndex);

		if (pSoundInfo)
		{
			::PlaySound(pSoundInfo->strFullPath, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
		}
	}
}

void CSoundsPropPage::OnButtonClickChangeSound(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	ChangeSound();
}

LRESULT CSoundsPropPage::OnDoubleClickListSelection(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	ChangeSound();

	return 0L;
}

void CSoundsPropPage::ChangeSound()
{
	int nIndex = m_wndListSounds.GetSelectedIndex();

	if (-1 != nIndex)
	{
		// Get initial directory path
		CString strInitialDir;

		LPSOUNDINFO pSoundInfo = (LPSOUNDINFO) m_wndListSounds.GetItemData(nIndex);

		ATLASSERT(pSoundInfo != NULL);
		if (pSoundInfo == NULL)
		{
			return;
		}

		if (FALSE == (pSoundInfo->strFullPath).IsEmpty())
		{
			CPath path(pSoundInfo->strFullPath);
			path.RemoveFileSpec();

			strInitialDir = path.m_strPath;
		}
		else
		{
			strInitialDir = m_pSettings->GetDefaultDirPath();
		}

		// Show choose file dialog
		TCHAR szPath[MAX_PATH + 1] = {0};

		CWindow tmpWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();
        CUIMFileDialog fileDialog(TRUE, _T("wav"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Sons (*.wav)|*.wav|"), tmpWindow.m_hWnd);
		fileDialog.m_ofn.lpstrInitialDir = strInitialDir;
		
		if ((IDOK == fileDialog.DoModal()) && (fileDialog.m_szFileName != NULL) && 
				(IsValidSoundFile(fileDialog.m_szFileName)))
		{
			::_tcsncpy(szPath, fileDialog.m_szFileName, MAX_PATH);
			
			pSoundInfo->strFullPath = szPath;
			pSoundInfo->bDirty = TRUE;

			CPath path(szPath);
			path.StripPath();

			CString strFileName = path.m_strPath;

			m_wndListSounds.SetItemText(nIndex, 1, strFileName);

			GetParent().SendMessage(WM_USER_PREFS_CHANGED);
		}
	}
}


//LRESULT CSoundsPropPage::OnItemChangedSoundList(NMHDR *pNMHDR)
//{
//	m_btnChangeSound.EnableWindow(TRUE);
//
//	return 0L;
//}


LRESULT CSoundsPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


void CSoundsPropPage::OnApply()
{
	if ( NL_MUTE & m_dwToNotify )
	{
        m_pGeneralSettings->EnableSound(m_btnSound.GetCheck()== BST_CHECKED ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerGeneralSettings::SD_MUTE);
	}

	if ( NL_MUTE_WHILE_AWAY	& m_dwToNotify )
	{
        m_pGeneralSettings->EnableMuteWhileAway(m_btnMuteWhileAway.GetCheck() == BST_CHECKED ? TRUE : FALSE);
		m_pSettings->NotifyEventListeners(IUOLMessengerGeneralSettings::SD_MUTE_WHILE_AWAY);
	}

	m_dwToNotify = 0;

	// Update settings
	int nCount = m_wndListSounds.GetItemCount();

	for (int nInd = 0; nInd < nCount; nInd++)
	{
		LPSOUNDINFO pSoundInfo = (LPSOUNDINFO) m_wndListSounds.GetItemData(nInd);

		if (pSoundInfo && pSoundInfo->bDirty)
		{
			m_pSettings->ChangeSoundEventFile((pSoundInfo->pSoundEvent)->GetEventType(), pSoundInfo->strFullPath);

			// Update full path
			pSoundInfo->strFullPath = (pSoundInfo->pSoundEvent)->GetFilePath();
			pSoundInfo->bDirty = FALSE;
		}
	}

	m_pSettings->NotifyObservers(&CUOLMessengerSoundSettingsObserver::OnSoundSettingsChanged, m_pSettings);
}


BOOL CSoundsPropPage::IsValidSoundFile(TCHAR* fileName)
{
	BOOL isFileOk = FALSE;
	
	CPath path(fileName);
	path.StripPath();
	
	// For now, we will just check the extension.
	//
	if ( (path.FindExtension() == -1) || (path.GetExtension().CompareNoCase(".wav") != 0) )
	{
		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		CString strMessage;
		strMessage = pStringLoader->GetValue("uol.resource.error.IDS_WIZARD_INVALID_SOUND_FILE");
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogWarning(__FUNCTION__, 
				"O arquivo [%s] de som é inválido. Somente o formato (*.wav) é aceito.", 
				CString(fileName));
		
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
	}
	else
	{
		isFileOk = TRUE;
	}
	
	return isFileOk;
}


void CSoundsPropPage::SortSoundEventList(CAtlList<IUOLMessengerSoundEventPtr>* pListSoundEvents)
{
	int nSoundEventsCount = (int) pListSoundEvents->GetCount();

	if (nSoundEventsCount > 0)
	{
		SoundEventHelper* pSoundEventHelper;
		int nInd = 0;
		SoundEventHelper** ppSoundEvents = new SoundEventHelper* [nSoundEventsCount];

		while (pListSoundEvents->GetCount() > 0)
		{
			pSoundEventHelper = new SoundEventHelper;
			pSoundEventHelper->pSoundEvent = pListSoundEvents->RemoveHead();
			ppSoundEvents[nInd] = pSoundEventHelper;
			nInd++;
		}

		ATLASSERT(nInd == nSoundEventsCount);

		::qsort(ppSoundEvents, nSoundEventsCount, sizeof(SoundEventHelper*), CompareSoundEvent);

		for (nInd = 0; nInd < nSoundEventsCount; nInd++)
		{
			pSoundEventHelper = ppSoundEvents[nInd];
			pListSoundEvents->AddTail(pSoundEventHelper->pSoundEvent);
			delete pSoundEventHelper;
		}

		ATLASSERT(nInd == nSoundEventsCount);

		delete ppSoundEvents;
	}
}


int CSoundsPropPage::CompareSoundEvent(const void* pSoundEventPointer1, const void* pSoundEventPointer2)
{
	SoundEventHelper* pSoundEvent1 = *((SoundEventHelper**)pSoundEventPointer1);
	SoundEventHelper* pSoundEvent2 = *((SoundEventHelper**)pSoundEventPointer2);

	if (pSoundEvent1->pSoundEvent->GetDescription() > pSoundEvent2->pSoundEvent->GetDescription())
	{
		return -1;
	}
	else if (pSoundEvent1->pSoundEvent->GetDescription() < pSoundEvent2->pSoundEvent->GetDescription())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void CSoundsPropPage::UpdateSoundEventList()
{
	// Fill columns "event type" and "file path"...
	CAtlList<IUOLMessengerSoundEventPtr> listSoundEvents;
	m_pSettings->GetSoundEventList(listSoundEvents);
	SortSoundEventList(&listSoundEvents);

	m_wndListSounds.DeleteAllItems();

	int nInd = 0;

	for (POSITION pos = listSoundEvents.GetHeadPosition(); pos;)
	{
		IUOLMessengerSoundEventPtr pSoundEvent = listSoundEvents.GetNext(pos);

		if (pSoundEvent)
		{
			// Get sound full path
			CString strFilePath = pSoundEvent->GetFilePath();

			// Is path relative?
			if (ATLPath::IsRelative(strFilePath))
			{
				strFilePath = m_pSettings->GetDefaultDirPath() + _T("\\") + strFilePath;
			}
			
			LPSOUNDINFO pSoundInfo = new SOUNDINFO();

			pSoundInfo->pSoundEvent = pSoundEvent;
			pSoundInfo->strFullPath = strFilePath;
			pSoundInfo->bDirty = FALSE;

			// Get file name only
			CPath pathShort(strFilePath);
			pathShort.StripPath();

			m_wndListSounds.InsertItem(nInd, pSoundEvent->GetDescription());
			m_wndListSounds.SetItemText(nInd, 1, pathShort.m_strPath);
			m_wndListSounds.SetItemData(nInd, (DWORD_PTR) pSoundInfo);
		}
	}
}