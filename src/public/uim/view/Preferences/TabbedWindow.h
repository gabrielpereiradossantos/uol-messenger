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

#include "../skin/WindowElement.h"
#include "../DummyWindow.h"
#include "../skin/BitmapBackgroundElement.h"

#include "TabbedWindowConstants.h"
#include "ButtonTabCtrl.h"
#include "PreferencesPropPage.h"
#include "../../resource.h"
#include "../UOLToolbar.h"
#include "../../controller/CommandFactory.h"
#include "../../controller/PropertyPageCommandFactory.h"
#include "../../controller/PluginPreferencesPageContainer.h"


class CTabbedWindow;
typedef CWindowElement<CTabbedWindow, CDummyWindow<CWindowImpl<CTabbedWindow> > > CParentClass;

class CTabbedWindow : public CParentClass,
					  public CMessageFilter,
					  public CUOLMessengerImageButtonEventListener2,
					  public CPluginPreferencesPageContainerObserver

{
public:
	DECLARE_WND_CLASS("TabbedWindow");

	CTabbedWindow(LPCTSTR lpszTitle = NULL, int nActivePage = 0, HWND hWndParent = NULL);
	virtual ~CTabbedWindow();

	BOOL RemovePage(HWND hWndPage);
	void RemoveAll();

	int GetPageCount();
	int GetActivePageIndex();
	int GetPageIndex(const CString& strPageTitle);

	void SetActivePageIndex(int nIndex);
	void SetActivePage(HWND hWndPage);
	void SetTitle(LPCTSTR lpszTitle);
	void RemovePluginPage(const CString& strPluginName);

	BOOL HandlePropPageCommand(IUOLMessengerPropertyPageCommandPtr pPropPageCommand);
	BOOL PreTranslateMessage(MSG* pMsg);
	
	IUOLMessengerImageButtonPtr CreateToolbarButton();

protected:
	BOOL AddPage(HWND hWndPage);
	BOOL RemovePage(int nIndex);
	BOOL CreatePages();

	struct PageInfo
	{
		CWindow wndPage;

		bool operator==(const PageInfo& pageInfo) const
		{
			return (wndPage == pageInfo.wndPage);
		}

		PageInfo& operator=(const PageInfo& pageInfo)
		{
			wndPage = pageInfo.wndPage;
			return *this;
		}
	};

	MAKEAUTOPTR(PageInfo);

protected:
	int GetPageIndex(HWND hWndPage);
	void GetPageRect(CRect& rect);

	BEGIN_MSG_MAP(CTabbedWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)		
		COMMAND_HANDLER(ID_PREF_OK, BN_CLICKED, OnOkBtnClicked)
		COMMAND_HANDLER(ID_PREF_CANCEL, BN_CLICKED, OnCancelBtnClicked)
		COMMAND_HANDLER(ID_PREF_APPLY, BN_CLICKED, OnApplyBtnClicked)
		MESSAGE_HANDLER_EX(WM_USER_PREFS_CHANGED, OnPrefsChanged);
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CParentClass)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCs);
	void OnDestroy();	
	void UpdateLayout(BOOL bResizeBars = TRUE);	

	LRESULT OnOkBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancelBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnApplyBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPrefsChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// CImageButtonEventListener overridden functions
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);

	// CPluginPreferencesPageContainerObserver events
	virtual void OnAddPage(IUOLMessengerPluginPreferencesPagePtr pPluginPage);
	virtual void OnRemovePage(IUOLMessengerPluginPreferencesPagePtr pPluginPage);


private:
	void UpdateActivePageIndex(int nIndex);
	void LoadThemeImpl(BOOL bRedraw);
	void ApplyChanges();
	void CreateToolbar();
	void AddAllPluginsPreferencesPage();
	void AddPluginsPreferencesPage(IUOLMessengerPluginPreferencesPagePtr pPluginPage);
	void RemovePluginsPreferencesPage(IUOLMessengerPluginPreferencesPagePtr pPluginPage);


protected:
	CSimpleArray<PageInfoPtr>					m_arrPages;
	CAtlList<IUOLMessengerPreferencesPagePtr>	m_listPropPages;
	CString				m_strTitle;
	CWindow				m_wndParent;
	int					m_nActivePage;
	CImageButtonBase*	m_pActiveButton;

	CUOLToolBarPtr		m_pToolbar;

	CSize				m_szTabItem;

	WTL::CFont			m_fontButton;

	WTL::CButton		m_btnOk;
	WTL::CButton		m_btnCancel;
	WTL::CButton		m_btnApply;
	
	CAtlList<IUOLMessengerPreferencesPagePtr>	m_listTempPluginPropPages;

	CPluginPreferencesPageContainerPtr m_pPluginPreferencesPageContainer;
};
