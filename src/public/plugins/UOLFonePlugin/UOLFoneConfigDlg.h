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


#include "resource.h"
#include "UOLFonePluginConfig.h"
#include "Core\UOLFoneController.h"
#include "COLORREF.h"
#include "Core\TooltipButton.h"

#include <interfaces/IUOLMessengerImage.h>


class CUOLFoneConfigDlg : 
		public CDialogImpl<CUOLFoneConfigDlg>,
		public CWinDataExchange<CUOLFoneConfigDlg>,
		public IUOLMessengerPreferencesPage2
{
public:
	
	enum { IDD = IDD_CONFIG_DLG };

	CUOLFoneConfigDlg(const CString& strTabName, const CString& strPluginInternalName,
		const IUOLMessengerImagePtr& pImage);
	virtual ~CUOLFoneConfigDlg();
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneConfigDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT,WM_CTLCOLORSTATIC , OnCtlColor)
		COMMAND_ID_HANDLER(IDC_BTN_LIMPAR_HISTORICO, OnClearHistory)
		COMMAND_HANDLER(IDC_CMB_CONTAS, CBN_DROPDOWN, OnComboDropDown)
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE, OnCbnSelChange)
		MESSAGE_HANDLER(WM_TAB_SETACTIVE, OnSetActive)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_FORCE_TUNNEL, OnCheckButtonClicked)
		COMMAND_HANDLER_EX(IDC_EDIT_PURGE_INTERVAL, EN_CHANGE, OnChangePurgeInterval)
		COMMAND_ID_HANDLER_EX(IDC_CHECK_REGISTER_ON_CALL, OnCheckButtonClicked)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CUOLFoneConfigDlg)
		DDX_CONTROL_HANDLE(IDC_CMB_CONTAS, m_callAccountCombo)
		DDX_CONTROL_HANDLE(IDC_CMB_ENTRADA_AUDIO, m_lineInDeviceCombo)
		DDX_CONTROL_HANDLE(IDC_CMB_SAIDA_AUDIO, m_lineOutDeviceCombo)
		DDX_CONTROL_HANDLE(IDC_CHECK_FORCE_TUNNEL, m_forceTunnelButton)
		DDX_CONTROL_HANDLE(IDC_EDIT_PURGE_INTERVAL, m_editPurgeInterval)
		DDX_CONTROL_HANDLE(IDC_SPIN_PURGE_INTERVAL, m_updownPurgeInterval)
		DDX_CONTROL_HANDLE(IDC_CHECK_REGISTER_ON_CALL, m_registerOnCallButton)
		DDX_TEXT(IDC_CMB_CONTAS, m_strCallAccount)
		DDX_TEXT(IDC_CMB_ENTRADA_AUDIO, m_strLineInDevice)
		DDX_TEXT(IDC_CMB_SAIDA_AUDIO, m_strLineOutDevice)
		DDX_CHECK(IDC_CHECK_FORCE_TUNNEL, m_bForceTunnel)
		DDX_CHECK(IDC_CHECK_REGISTER_ON_CALL, m_bRegisterOnCall)
	END_DDX_MAP()
	
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClearHistory(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnComboDropDown(int /*HiWordWParam*/, int /*LowWordWParam*/, HWND /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy();
	LRESULT OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCbnSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	void CreateColors();
	LRESULT OnSetActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnCheckButtonClicked(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnChangePurgeInterval(UINT /*uCode*/, int nID, HWND hwndCtrl);

	// IUOLMessengerPluginPreferencesTab methods
	virtual CString GetTabName() const;
	virtual CString GetPluginInternalName() const;
	virtual void GetMinSize(CSize& size);
	virtual HWND CreateTab(HWND hWndParent, const CRect& rectTab, DWORD dwWindowStyle = WS_CHILDWINDOW);
	virtual void DestroyTab(HWND hWndTab);
	virtual HWND GetTabHWND();
	virtual IUOLMessengerImagePtr GetTabImage();
	virtual void SetTabButtonIndex(int nTabIndex);
	virtual int  GetTabButtonIndex();
	virtual void Show(int nCmdShow);

public:
	//IUOLMessengerPreferencesPage2
	virtual void OnApply();
	virtual HWND GetHWND();
	virtual HWND Create(HWND hWndParent);

private:
	void FillAccountListCombo();
	void FillInputDevicesCombo();
	void FillOutputDevicesCombo();
	
private:
	WTL::CBrush		m_brushEdit;
	WTL::CBrush		m_brushListBox;
	WTL::CBrush		m_brushDialog;
	WTL::CBrush		m_brushButton;
	WTL::CBrush		m_brushStatic;
	WTL::CBrush		m_brushScrollBar;
	CCOLORREF		m_clrbrushEdit;
	CCOLORREF		m_clrbrushListBox;
	CCOLORREF		m_clrbrushDialog;
	CCOLORREF		m_clrbrushButton;
	CCOLORREF		m_clrbrushScrollBar;
	CCOLORREF		m_clrbrushStatic;


	CComboBox				m_callAccountCombo;
	CComboBox				m_lineInDeviceCombo;
	CComboBox				m_lineOutDeviceCombo;
	CButton					m_forceTunnelButton;
	CEdit					m_editPurgeInterval;
	CUpDownCtrl				m_updownPurgeInterval;
	CButton					m_registerOnCallButton;
	CTooltipButton			m_registerOnCallTooltipButton;
	
	CString					m_strCallAccount;
	CString					m_strLineInDevice;
	CString					m_strLineOutDevice;
	BOOL					m_bForceTunnel;
	BOOL					m_bRegisterOnCall;

	CUOLFoneController*		m_pUOLFoneController;
	CUOLFonePluginConfig*	m_pUOLFonePluginConfig;
    IUOLMessengerFacade*	m_pUOLMessengerFacade;

	struct AccountHelper
	{
		IUOLMessengerAccountPtr	pAccount;
	};

	enum NotifyListener
	{
		NL_CALL_ACCOUNT		= 1,
		NL_LINE_IN_DEVICE	= 2,
		NL_LINE_OUT_DEVICE	= 4,
		NL_FORCE_TUNNEL		= 8,
		NL_PURGE_INTERVAL	= 16,
		NL_REGISTER_ON_CALL	= 32
	};

	DWORD m_dwToNotify;

	// Plugin elements.
	CString		m_strTabName;
	CString		m_strPluginInternalName;
	
	IUOLMessengerImagePtr m_pImage;

	int			m_nTabIndex;
};

MAKEAUTOPTR(CUOLFoneConfigDlg);
