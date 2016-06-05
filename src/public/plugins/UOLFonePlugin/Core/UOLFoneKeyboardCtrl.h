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
#include "UOLFoneMultiStateButton.h"
#include "UOLFoneChildWindowInfo.h"


///////////////////////////////////////////////////////////////////////////////
// CUOLFoneKeyboardCtrlObserver Class
///////////////////////////////////////////////////////////////////////////////
class CUOLFoneKeyboardCtrlObserver
{
public:
	virtual ~CUOLFoneKeyboardCtrlObserver() {}

	virtual void OnFoneKeyboardPressed(UINT nChar, BOOL bCleanLast) = 0;
};


///////////////////////////////////////////////////////////////////////////////
// CUOLFoneKeyboardCtrl Class
///////////////////////////////////////////////////////////////////////////////
class CUOLFoneKeyboardCtrl : public CWindowImpl<CUOLFoneKeyboardCtrl, CWindow>
{
public:
	DECLARE_WND_CLASS(NULL)
	
	CUOLFoneKeyboardCtrl();
	virtual ~CUOLFoneKeyboardCtrl();

	int GetWidth();
	int GetHeight();
	void SetAlfaNumeric(BOOL bIsAlfaNumeric);
	void RegisterObserver(CUOLFoneKeyboardCtrlObserver* pObserver);
	void UnregisterObserver(CUOLFoneKeyboardCtrlObserver* pObserver);
	void PressFoneKeyboardKey(UINT nChar);
		
protected:
	enum
	{
		ID_BUTTON_KEY_0 = 0x2514,
		ID_BUTTON_KEY_1,
		ID_BUTTON_KEY_2,
		ID_BUTTON_KEY_3,
		ID_BUTTON_KEY_4,
		ID_BUTTON_KEY_5,
		ID_BUTTON_KEY_6,
		ID_BUTTON_KEY_7,
		ID_BUTTON_KEY_8,
		ID_BUTTON_KEY_9,
		ID_BUTTON_KEY_ASTERISK,
		ID_BUTTON_KEY_POUND
	};
	
	BEGIN_MSG_MAP(CUOLFoneKeyboardCtrl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_RANGE_HANDLER_EX(ID_BUTTON_KEY_0, ID_BUTTON_KEY_POUND, OnButtonClicked)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnButtonClicked(UINT uCode, int nButtonId, HWND hwndCtrl);
	LRESULT OnChar(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SendButtonClickedMessage(WORD ctrlID, HWND ctrlHandle);
	void NotifyAllFoneKeyboardPressed(UINT nChar);

	// Methods to control alfa-numeric keyboard
	void UpdateAlfaNumericButtonControl(int nButtonId);
	int GetNextAlfaNumericButtonSequence(int nButtonId, int iSequence);
	void NotifyAlfaNumericButtonChar(int nButtonId);
	BOOL DoesAlfaNumericButtonAllowSequence(int nButtonId);
	
	
private:
	WTL::CButton		m_key0;
	WTL::CButton		m_key1;
	WTL::CButton		m_key2;
	WTL::CButton		m_key3;
	WTL::CButton		m_key4;
	WTL::CButton		m_key5;
	WTL::CButton		m_key6;
	WTL::CButton		m_key7;
	WTL::CButton		m_key8;
	WTL::CButton		m_key9;
	WTL::CButton		m_keyAsterisk;
	WTL::CButton		m_keyPound;
	
	CUOLFoneMultiStateButton	m_multiStateKey0;
	CUOLFoneMultiStateButton	m_multiStateKey1;
	CUOLFoneMultiStateButton	m_multiStateKey2;
	CUOLFoneMultiStateButton	m_multiStateKey3;
	CUOLFoneMultiStateButton	m_multiStateKey4;
	CUOLFoneMultiStateButton	m_multiStateKey5;
	CUOLFoneMultiStateButton	m_multiStateKey6;
	CUOLFoneMultiStateButton	m_multiStateKey7;
	CUOLFoneMultiStateButton	m_multiStateKey8;
	CUOLFoneMultiStateButton	m_multiStateKey9;
	CUOLFoneMultiStateButton	m_multiStateKeyAsterisk;
	CUOLFoneMultiStateButton	m_multiStateKeyPound;
	
	CWTLWndComponentsOrganizer	m_componentsOrganizer;
	
	BOOL						m_destroyingWindow;
	CComAutoCriticalSection		m_destroyWindowLock;

	CAtlList<CUOLFoneKeyboardCtrlObserver*>  m_listObservers;

	// Variables to control alfa-numeric keyboard
	int		m_iLastAlfaNumericButton;
	int		m_iAlfaNumericButtonSequence;
	DWORD	m_dwAlfaNumericButtonTick;
	BOOL	m_bCleanLast;
	BOOL	m_bIsAlfaNumeric;
};

MAKEAUTOPTR(CUOLFoneKeyboardCtrl);

