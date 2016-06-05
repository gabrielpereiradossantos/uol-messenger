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

#include "TabbedWindowConstants.h"
#include "../skin/Element.h"

#include <interfaces/IUOLMessengerImage.h>
#include <interfaces/IUOLMessengerPreferencesPage.h>


#include "../Skin/COLORREF.h"
#include "../image/image.h"
#include "../Image/imageBuilder.h"

class CTabbedWindow;

class CPreferencesPropPage : public CDialogImpl<CPreferencesPropPage, CWindow>,
							 public CElement,
							 public IUOLMessengerPreferencesPage
{
	BEGIN_MSG_MAP(CPreferencesPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT,WM_CTLCOLORSTATIC , OnCtlColor)
		//MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MESSAGE_HANDLER(WM_TAB_SETACTIVE, OnSetActive)
		MESSAGE_HANDLER(WM_TAB_KILLACTIVE, OnKillActive)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	CPreferencesPropPage(DWORD dwIDD, const CString& strTitle = _T(""));
	virtual ~CPreferencesPropPage();

	HWND Create(CTabbedWindow* pParent, LPARAM lParam);

	void SetTitle(const CString& strTitle);
	CString GetTitle() const;

	void SetIcon(IUOLMessengerImagePtr pIcon);
	IUOLMessengerImagePtr GetIcon() const;

	virtual void OnSetActive();
	virtual void OnKillActive();
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

	void SetTabbedWindow(CTabbedWindow* pTabbedWindow);
	CTabbedWindow* GetTabbedWindow() const;

	//IUOLMessengerPreferencesPage
	virtual void OnApply(){};
	virtual HWND GetHWND(){return m_hWnd;};

protected:

	LRESULT OnInitDialog(HWND, LPARAM);
	void OnClose();
	LRESULT OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlStaticColor(HDC hdc, HWND hwnd);
	LRESULT OnSetActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillActive(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	HWND Create(LPCTSTR lpstrWndClass, HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);

public:
	DWORD	IDD;


protected:
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

private:
	IUOLMessengerImagePtr	m_pIcon;
	CString m_strTitle;	
	CTabbedWindow*	m_pTabbedWindow;
};

MAKEAUTOPTR(CPreferencesPropPage);