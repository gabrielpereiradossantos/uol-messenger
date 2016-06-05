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

#include "../skin/windowelement.h"
#include "../DummyWindow.h"

#include ".\uolproppage.h"

#define ID_BTNCUSTOM		42

#define ID_CREATEPROFILE	100


class CUOLWizard;
typedef CWindowElement<CUOLWizard, CDummyWindow<CDialogImpl<CUOLWizard> > > _CUOLWizardBase;

class CUOLWizard : public _CUOLWizardBase 
{
public:
	CUOLWizard(int nIDD);
	virtual ~CUOLWizard(void);

	const int IDD;

	void SetInitialRect(const CRect& rect);
	void SetCenterWindow(BOOL bCenterWindow = TRUE);

	void SetPage(CUOLPropPagePtr pWindow);	

	int GetDefaultButtonHeight() const;
	int GetDefaultButtonSpacement() const;
	int GetDefaultButtonWidth() const;

    void ShowBtnPrevious(BOOL bShow);
	void ShowBtnNext(BOOL bShow);
	void ShowBtnCancel(BOOL bShow);

    void CreateBtnCustom(int x, int width);
	void ShowBtnCustom(BOOL bShow, const CString& strText);

	void SetDefaultButton(int nIdButton);

	void ShowNextPage();

	UOLLib::ISettings* GetWizardSettings();
	
protected:
	BEGIN_MSG_MAP(CUOLWizard)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(ID_PREV_PANE, OnPrev)
		COMMAND_ID_HANDLER_EX(ID_NEXT_PANE, OnNext)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		MSG_WM_COMMAND(OnCommand)
		CHAIN_MSG_MAP(_CUOLWizardBase)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);

	void OnPrev(UINT, int, HWND);
	void OnNext(UINT, int, HWND);
	void OnCancel(UINT, int, HWND);
	void OnCommand(UINT, int, HWND);	

private:
	void CreateBtns();
	void SetupBtns();
	void CreatePropWindow(CUOLPropPagePtr pWindow);
	void DestroyPropWindow(CUOLPropPage* pWindow);
	void CloseAllProps();

protected:
	CRect		m_rectInitial;
	BOOL		m_bCenterWindow;

	CUOLPropPagePtr	m_pWindow;	

	WTL::CButton	m_btnPrev;
	WTL::CButton	m_btnNext;
	WTL::CButton	m_btnCancel;
	WTL::CButton	m_btnCustom;

	CFont m_BtnFont;

	// This list is used to keep the prop pages in memoty
	CAtlList<CUOLPropPagePtr>	m_listPropPages;

	UOLLib::ISettings*	m_pWizardSettings;
};

MAKEAUTOPTR(CUOLWizard);