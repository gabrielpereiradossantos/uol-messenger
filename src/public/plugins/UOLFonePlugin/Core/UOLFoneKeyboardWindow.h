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

#include "WTLWndComponentsOrganizer.h"
#include "IUOLMessengerPluginCustomTabChildWindow.h"
#include "UOLFoneMultiStateButton.h"
#include "UOLFoneChildWindowInfo.h"
#include "PhoneNumberComboBox.h"
#include "UOLFoneTooltipLabel.h"
#include "UOLFoneKeyboardCtrl.h"


class CUOLFoneKeyboardWindow;

typedef CWinTraits<WS_CHILD | WS_CLIPCHILDREN, WS_EX_TOPMOST> CTopMostControlWinTraits;


class CUOLFoneKeyboardWindow : 
		public CWindowImpl<CUOLFoneKeyboardWindow, CWindow, CTopMostControlWinTraits>,
		public IUOLMessengerPluginCustomTabChildWindow,
		public CUOLFoneKeyboardCtrlObserver,
		public CPhoneNumberComboBoxObserver
{
public:
	
	DECLARE_WND_CLASS(NULL)
	
	CUOLFoneKeyboardWindow();
	virtual ~CUOLFoneKeyboardWindow();
	
	virtual void GetMinSize(CSize& size);
	virtual void UpdateAccountInfo(CUOLFoneAccountPtr pAccount);
	virtual void UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall);
	virtual void SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo);
	virtual BOOL CanGetFocus();
	virtual void ResetWindow();
	
	void GetPhoneNumberToCall(CString& phoneNumber);
	void GetDigitedText(CString& pDigitedText);

	// CUOLFoneKeyboardCtrlObserver
	virtual void OnFoneKeyboardPressed(UINT nChar, BOOL bCleanLast);

	// CPhoneNumberComboBoxObserver
	virtual void OnPhoneNumberComboBoxKeyPressed(TCHAR ch);
	
protected:
	enum
	{
		ID_BUTTON_FAKE_CMD_ID = 0x2520, 
		ID_BUTTON_CLEAN_CALL_NUMBER
	};
	
	BEGIN_MSG_MAP(CUOLFoneKeyboardWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_RANGE_HANDLER_EX(ID_BUTTON_FAKE_CMD_ID, ID_BUTTON_CLEAN_CALL_NUMBER, OnButtonClicked)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, OnCtlColor)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		COMMAND_CODE_HANDLER_EX(CBN_DROPDOWN, OnCbnDropDown)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	LRESULT OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCbnDropDown(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnCtlColorStatic(HDC hDC, HWND hWnd);
	
private:
	WTL::CStatic		m_makeCallTo;
	CPhoneNumberComboBox	m_phoneNumberCombo;
	WTL::CStatic	m_howtoCallLine1;
	WTL::CStatic	m_howtoCallLine2;
	WTL::CStatic	m_howtoCallLine3;
		
	WTL::CFont		m_normalFont;
	WTL::CFont		m_smallFont;
	
	CWTLWndComponentsOrganizer	m_componentsOrganizer;
	
	BOOL						m_destroyingWindow;
	CComAutoCriticalSection		m_destroyWindowLock;

	CUOLFoneKeyboardCtrl		m_FoneKeyboardCtrl;
};

MAKEAUTOPTR(CUOLFoneKeyboardWindow);

