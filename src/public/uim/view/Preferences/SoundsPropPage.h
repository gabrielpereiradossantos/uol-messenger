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

#include "PreferencesPropPage.h"
#include "../../resource.h"
#include "../../model/SoundSettings.h"
#include <interfaces/IUOLMessengerGeneralSettings.h>


// CSoundsPropPage dialog

class CSoundsPropPage : public CPreferencesPropPage
{
public:
	CSoundsPropPage();
	virtual ~CSoundsPropPage();

	virtual void OnApply();

protected:
	BEGIN_MSG_MAP(CSoundsPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		//MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_SOUND, OnCheckButtonClicked)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_MUTE_WHILE_AWAY, OnCheckButtonClicked)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CHANGE_SOUND, OnButtonClickChangeSound)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_PLAY_SOUND, OnButtonClickPlaySound)
		NOTIFY_HANDLER(IDC_LIST_SOUNDS, NM_DBLCLK, OnDoubleClickListSelection)
		//NOTIFY_HANDLER_EX(IDC_LIST_SOUNDS, LVN_ITEMCHANGED, OnItemChangedSoundList)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnDestroy();
	LRESULT OnShowWindow(BOOL bShow, int nStatus);
	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	void OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnButtonClickChangeSound(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnButtonClickPlaySound(UINT /*uCode*/, int nID, HWND hwndCtrl);
	LRESULT OnDoubleClickListSelection(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	void ChangeSound();
	//LRESULT OnItemChangedSoundList(NMHDR *pNMHDR);
	BOOL IsValidSoundFile(TCHAR* fileName);

	void SortSoundEventList(CAtlList<IUOLMessengerSoundEventPtr>* pListSoundEvents);
	static int CompareSoundEvent(const void* pSoundEventPointer1, const void* pSoundEventPointer2);
	void UpdateSoundEventList();

protected:
	enum GRID_INDEX
	{
		GI_NEW_MESSAGE = 0,
		GI_CONTACT_ONLINE = 1,
		GI_FILE_PATH = 1
	};

	CSoundSettingsPtr	m_pSettings;
	IUOLMessengerGeneralSettingsPtr	m_pGeneralSettings;

	CButton			m_btnSound;
	CButton			m_btnMuteWhileAway;
	CButton			m_btnChangeSound;
	CButton			m_btnPlaySound;
	
	CListViewCtrl	m_wndListSounds;
	CString		m_strColumnEventType;
	CString		m_strColumnFilePath;
	CString		m_strNewMessageSound;
	CString		m_strOnlineSound;

	enum NotifyListener
	{
		NL_MUTE						= 1,
		NL_MUTE_WHILE_AWAY			= 2,
		NL_CHANGED_CONTACT_ONLINE	= 4,
		NL_CHANGED_NEW_MESSAGE		= 8
	};

	DWORD m_dwToNotify;

};
MAKEAUTOPTR(CSoundsPropPage);