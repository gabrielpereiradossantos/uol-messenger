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
 
// ProfileDlg.h : Declaration of the CProfileDlg

#pragma once

#include "../resource.h"       // main symbols
#include "../controller/profilemanager.h"

#include "skin/windowelement.h"
#include "dummywindow.h"

#include ".\transparentstatic.h"
#include "../controller/UIMApplication.h"

// CProfileDlg

class CProfileDlg;
typedef CWindowElement<CProfileDlg, CDummyWindow<CDialogImpl<CProfileDlg> > > CProfileDlgBase;

class CProfileDlg : public CProfileDlgBase,
				    public CWinDataExchange<CProfileDlg>

{
public:
	CProfileDlg()
	{
		m_pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	}

	~CProfileDlg()
	{
	}

	BOOL ProfileDlgExecute(CProfileManager* profileManagerPtr);

	enum { IDD = IDD_DIALOG_PROFILE };

	BEGIN_MSG_MAP(CProfileDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnCloseDialog)				
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic) 
		MSG_WM_CTLCOLORBTN(OnCtlColorBtn) 
		NOTIFY_HANDLER_EX(IDC_RADIO_PROFILE_0, NM_CUSTOMDRAW, OnRadioCustomDraw)
		NOTIFY_HANDLER_EX(IDC_RADIO_PROFILE_1, NM_CUSTOMDRAW, OnRadioCustomDraw)
		COMMAND_HANDLER(IDC_BUTTON_OK, BN_CLICKED, OnClickedOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)
		COMMAND_HANDLER(IDC_BUTTON_REMOVE_PROFILE, BN_CLICKED, OnClickedRemove)
		COMMAND_HANDLER(IDC_RADIO_PROFILE_0, BN_CLICKED, OnRadioProfile)
		COMMAND_HANDLER(IDC_RADIO_PROFILE_1, BN_CLICKED, OnRadioProfile)
		COMMAND_HANDLER(IDC_BUTTON_CHANGE_DIR, BN_CLICKED, OnChangeDir)
		CHAIN_MSG_MAP(CProfileDlgBase)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CProfileDlg)
		DDX_CONTROL_HANDLE(IDC_COMBO_PROFILE, m_wndComboProfile)
		DDX_CONTROL_HANDLE(IDC_EDIT_PROFILE, m_wndEditProfile)
		DDX_CONTROL_HANDLE(IDC_BUTTON_CHANGE_DIR, m_wndButtonChangeDir)
		DDX_CONTROL_HANDLE(IDC_BUTTON_REMOVE_PROFILE, m_wndButtonRemoveProfile)
		DDX_RADIO(IDC_RADIO_PROFILE_0, m_nRadioProfile)
	END_DDX_MAP()


// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCloseDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnCtlColorStatic(HDC hDC, HWND hwnd);
	LRESULT OnCtlColorBtn(HDC hDC, HWND hwnd);
	LRESULT OnRadioCustomDraw(LPNMHDR lpNmhdr);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRadioProfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnChangeDir(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void InitDialog();
	void SelectEnabled(BOOL bEnabled);
	void NewEnabled(BOOL bEnabled);

private:
	CProfileManager* m_profileManagerPtr;

	CString m_strProfileBaseDir;

	CComboBox m_wndComboProfile;
	CButton m_wndButtonChangeDir;
	CButton m_wndButtonRemoveProfile;
	CEdit m_wndEditProfile;
	int		m_nRadioProfile;

	COLORREF			m_bkg;			
	WTL::CBrush			m_brushBkg;
	CTransparentStatic	m_staticTip;

	UOLLib::ISettings*   m_pStringLoader;
};


