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

#include "WMEventNotifier.h"
#include "UOLFoneMultiStateButton.h"


///////////////////////////////////////////////////////////////////////////////
// CPhoneNumberComboBoxObserver Class
///////////////////////////////////////////////////////////////////////////////
class CPhoneNumberComboBoxObserver
{
public:
	virtual ~CPhoneNumberComboBoxObserver() {}

	virtual void OnPhoneNumberComboBoxKeyPressed(TCHAR ch) = 0;
};


///////////////////////////////////////////////////////////////////////////////
// CPhoneNumberComboBox Class
///////////////////////////////////////////////////////////////////////////////
class CPhoneNumberComboBox : 
		public CWindowImpl<CPhoneNumberComboBox, CComboBoxEx>, 
		public CWMEventNotifier<CPhoneNumberComboBox>
{
public:
	static const UINT MAX_HISTORY_ITEMS = 20;

	CPhoneNumberComboBox(UINT nMaxItems = MAX_HISTORY_ITEMS);
	virtual ~CPhoneNumberComboBox();

	int AddToHistory(LPCTSTR lpszString);
	
	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
	BOOL DestroyWindow();
	
	void SetComboDefaultText(const CString& comboDefaultText);
	CString GetComboDefaultText();
	
	void SetInactiveTextColor(COLORREF textColor);
	void SetBkGrndColor(COLORREF bkColor);
	
	void SimulateKillFocus();

	void RegisterObserver(CPhoneNumberComboBoxObserver* pObserver);
	void UnregisterObserver(CPhoneNumberComboBoxObserver* pObserver);
	
protected:
	
	BEGIN_MSG_MAP(CPhoneNumberComboBox)
		CHAIN_MSG_MAP(CWMEventNotifier<CPhoneNumberComboBox>)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(CComboEdit::WM_BUTTON_CLEAN_CLICKED, OnCleanButtonClicked)
	END_MSG_MAP()
	
	virtual HBRUSH OnCtlColorEventNotifier(UINT uMsg, HDC hdc, HWND hWnd);
	
	LRESULT OnEraseBackground(HDC hDc);
	LRESULT OnCleanButtonClicked(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnSize(UINT nType, const CSize& size);
	
	class CComboEdit : 
			public CWindowImpl<CComboEdit, CEdit>, 
			public CWMEventNotifier<CComboEdit>
	{
	public:
		enum
		{
			WM_BUTTON_CLEAN_CLICKED = WM_USER + 0x234,
			ID_BUTTON_CLEAN_CALL_NUMBER = 0x2723
		};

		virtual ~CComboEdit();
		void SetParentWnd(HWND hwnd);
		void RegisterObserver(CPhoneNumberComboBoxObserver* pObserver);
		void UnregisterObserver(CPhoneNumberComboBoxObserver* pObserver);

		CString			m_comboDefaultText;
		CFont			m_fontSearchComboBox;
		CFont			m_fontSearchComboBoxText;
		
		COLORREF		m_rgbInactiveText;
		COLORREF		m_rgbBkGrnd;
		COLORREF		m_backupInactiveTextColor;
		WTL::CBrush		m_pBrush;
		HBRUSH			m_bkGrndBrushHandle;
		
		CFont			m_actualFont;
		BOOL			m_searchTextActive;
		
	protected:
		
		BEGIN_MSG_MAP(CComboEdit)
			CHAIN_MSG_MAP(CWMEventNotifier<CComboEdit>)
			MSG_WM_CHAR(OnChar)
			MSG_WM_PASTE(OnPaste)
			COMMAND_ID_HANDLER_EX(ID_BUTTON_CLEAN_CALL_NUMBER, OnCleanButtonClicked)
			REFLECT_NOTIFICATIONS()
		END_MSG_MAP()
		
		virtual void OnSetFocusEventNotifier(HWND hLostFocusWnd);
		virtual void OnKillFocusEventNotifier(HWND hGetFocusWnd);
		
		void OnChar(TCHAR ch, UINT, UINT);
		void OnPaste();
		void InitializeEditFeatures();
		void FinalizeEditFeatures();
		void UpdateEditLayout(BOOL settingFocus, BOOL editing);
		void OnCleanButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
		void NotifyAllKeyPressed(TCHAR ch);

	private:
		HWND m_hwndParent;
		CAtlList<CPhoneNumberComboBoxObserver*>  m_listObservers;
	};
	
	CComboEdit					m_comboEdit;		
	UINT						m_nMaxItems;
	WTL::CButton				m_cleanNumber;	
	CUOLFoneMultiStateButton	m_multiStateCleanNumber;
};
