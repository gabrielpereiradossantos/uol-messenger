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

#include <interfaces/IUOLMessengerFrameWindowView.h>
#include <interfaces/IUOLMessengerAccount.h>
#include "../skin/WindowElement.h"
#include "../skin/StaticIconElement.h"
#include "../DummyWindow.h"
#include "AccountListViewCtrl.h"
#include "../../controller/utils.h"
#include "../TransparentStatic.h"


class CConnectionErrorView;
//typedef CDummyWindow<CWindowImpl<CConnectionErrorView> > _CConnectionErrorViewBase;
typedef CWindowElement<CConnectionErrorView, CDummyWindow<CWindowImpl<CConnectionErrorView> > > _CConnectionErrorViewBase;


class CConnectionErrorView : 
	public IUOLMessengerFrameWindowView,
	public _CConnectionErrorViewBase
{
public:
	CConnectionErrorView();
	virtual ~CConnectionErrorView();

	void AddErrorEvent(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CTime& time = CTime::GetCurrentTime(), const CString& strMessage = "");
	void RemoveErrorEvent(IUOLMessengerAccountPtr pAccount);
	UINT GetErrorEventCount();
	void ReconnectAll();

	CString GetName() const;
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual void DoPaint(CDCHandle& dc);
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);

protected:
	virtual HWND CreateView(HWND hWndParent);
	virtual void GetMinSize(CSize& size);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CConnectionErrorView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		//MSG_WM_SIZE(OnSize)
		//MSG_WM_PAINT(OnPaint)
		//MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		//MSG_WM_CTLCOLORDLG(OnCtlColorDialog);
		//MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT,WM_CTLCOLORSTATIC , OnCtlColor)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_RECONNECT_ALL, OnButtonClickedReconnectAll)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CANCEL, OnButtonClickedCancel)
		NOTIFY_HANDLER_EX(IDC_LIST_VIEW_ACCOUNTS, LVN_ITEMCHANGED, OnItemChangedAccountList)
		CHAIN_MSG_MAP(_CConnectionErrorViewBase)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCs);
	void OnDestroy();
	//void OnSize(UINT nType, const CSize& size);
	//void OnPaint(HDC hDC);
	LRESULT OnEraseBackground(HDC hDC);
	LRESULT OnCtlColorStatic(HDC hDc, HWND);
	LRESULT OnCtlColorDialog(HDC hDc, HWND);
	void OnButtonClickedReconnectAll(UINT /*uCode*/, int nID, HWND hwndCtrl);
	void OnButtonClickedCancel(UINT /*uCode*/, int nID, HWND hwndCtrl);
	LRESULT OnItemChangedAccountList(NMHDR *pNMHDR);

protected:
	enum DialogControls
	{
		IDC_BUTTON_RECONNECT_ALL = 5000,
		IDC_BUTTON_CANCEL,
		IDC_LIST_VIEW_ACCOUNTS,
		IDC_STATIC_TITLE,
		IDC_STATIC_MESSAGE,
		IDC_STATIC_REASON_CAPTION,
	};

	enum DialogMetrics
	{
		DM_IMAGE_WIDTH = 64,
		DM_IMAGE_HEIGHT = DM_IMAGE_WIDTH,
		DM_INTERNAL_SPACE = 6,
		DM_CONTROL_VERTICAL_SPACE = 3,
		DM_CONTROL_HORIZONTAL_SPACE = 3,
		DM_STATIC_BASE_HEIGHT = 20,
		DM_ACCOUNT_LIST_HEIGHT = 110,
		DM_BUTTON_RECONNECT_WIDTH = 120,
		DM_BUTTON_CANCEL_WIDTH = 80,
		DM_BUTTON_BASE_HEIGHT = 24
	};

private:
	void CreateControls();
	void ResizeControls(const CSize& size);
	void CloseParentFrame();
	int RemoveErrorEventImpl(IUOLMessengerAccountPtr pAccount);
	void OnItemChangedAccountListImpl();

private:
	CStringGUID				m_strGUID;

	CAccountListViewCtrl	m_wndAccountListView;
	//CTransparentStatic2		m_wndStaticMessage1;
	CStatic					m_wndStaticReasonMessage;
	CFont			m_fontNormal;
	CFont			m_fontTitle;

	CStaticIconElementPtr	m_pErrorIcon;
	CRect					m_rectImage;

	CBrush		m_brushBackground;

	CFontElementPtr			m_pFontWindow;
	CFontElementPtr			m_pFontTitle;

	CComAutoCriticalSection		m_csAccountListView;
};

MAKEAUTOPTR(CConnectionErrorView);
