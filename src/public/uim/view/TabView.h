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
 
// TabView.h : interface of the CTabView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <interfaces/IUOLMessengerConversation.h>
#include <atlcoll.h>
#include "skin/windowelement.h"
#include "dummywindow.h"
#include "skin/FontElement.h"
#include "image/image.h"
#include "image/imagebuilder.h"
#include "Widget.h"
#include "skin/TabViewElement.h"
#include "interfaces/IUOLMessengerTabViewCtrl.h"
#include "../controller/utils.h"

class CTabView;

class CTabViewEventObserver : public IUOLMessengerTabViewCtrlObserver2
{
public:
	//IUOLMessengerTabViewCtrlObserver interface
	virtual void OnConversationTabSelected(	IUOLMessengerTabViewCtrl* pTabView,
											IUOLMessengerConversationPtr pConversation, 
											int nTabIndex) {}

	//IUOLMessengerTabViewCtrlObserver2 interface
	virtual void OnConversationTabMouseHover(IUOLMessengerTabViewCtrl* pTabView,
											 IUOLMessengerConversationPtr pConversation, 
											 CPoint& pt, HWND wHandle) {}
	virtual void OnConversationTabMouseLeave() {}
};


class CWidget : public CWidgetBase
{
public:
	
	enum tabState {Normal=0,MouseOver,MouseClicked,Blinking,Selected};
	enum clickItem {clickNone=0, clickWidget, clickClose };

	CWidget (CTabViewButtonElementPtr& pTabViewButton,
			 IUOLMessengerImagePtr& pCloseTabImage,
			 CFontElementPtr& pFont,
			 CFontElementPtr& pFontSelected, 
			 int nWidgetIndex,
			 CTabView* pTabView = NULL);
	
	IUOLMessengerConversationPtr m_pConversation;
	CRect m_rectWidget;
	CRect m_rectClose;
	CRect m_rectText;
	CRect m_rectBtnImage;
	CString m_caption;
	tabState m_state;
	
	virtual void	SetRect(const CRect &rect);
	virtual int		PtInWidget(const CPoint &point );
	void	SetState(tabState state) { m_state = state; } 
	// TODO: // Herdar e utilizar om metodo DrawWidget(CDCHandle * dc)
	void	DrawWidget(CDCHandle * dc, BOOL selected, BOOL blinking/*, BOOL*/ );
	int		GetWidgetIndex();
	void	SetTabButtonImage(IUOLMessengerImagePtr&  pButtonTabImage);
	int		CalculateTabWidth(HDC windowDC);
    void    SetTabChanged(BOOL bTabChanged);
private:
	//void	DrawMouseOver(CDCHandle * dc);
	//void	DrawCliked(CDCHandle * dc);
	//void	DrawNormal(CDCHandle * dc);
	//void	DrawSelected(CDCHandle * dc);
	//void	DrawBlinking(CDCHandle * dc);

	void	DrawIt(	CDCHandle * dc,
		CImage* pLeftImage,
		IUOLMessengerImagePtr& pFillImage,
		CImage* pRightImage);
	
	void GetWidgetTextSize(HDC hDC, CSize& size, BOOL selected);
	
	CFontElementPtr&		m_pFont;
	CFontElementPtr&		m_pFontSelected;
	IUOLMessengerImagePtr&  m_pCloseTabImage;
	IUOLMessengerImagePtr   m_pButtonTabImage;
	
	CTabViewButtonElementPtr& m_pTabViewButton;
	IUOLMessengerImageDrawStrategyPtr m_pStrategy;
	int						m_nWidgetIndex;
	
	BOOL					m_showCloseButton;
	int						m_buttonLeftMargin;
	int						m_buttonTopMargin;
	int						m_buttonRightMargin;
	int						m_buttonBottomMargin;
	
	int						m_nTabWidth;
	BOOL					m_bTabChanged;
	CSize					m_widgetTextSize;
};




typedef CWindowElement<CTabView, CDummyWindow<CWindowImpl<CTabView> > > CTabViewBase;

class CTabView : 
		public CTabViewBase, 
		public IUOLMessengerTabViewCtrl
{
public:
	DECLARE_WND_CLASS(NULL)
	CTabView();
	~CTabView();
	BOOL PreTranslateMessage(MSG* pMsg);
	CString GetName() const;

	static const int MIN_WIDHT = 50;  //30;
	static const int MAX_WIDHT = 200; //100;
	static int const BORDER = 8;

	BEGIN_MSG_MAP(CTabView)
		MSG_WM_CREATE(OnCreate)				
		MSG_WM_TIMER(OnTimer)	
		MSG_WM_DESTROY(OnDestroy)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		CHAIN_MSG_MAP(CTabViewBase)
	END_MSG_MAP()

	// WindowElement methods
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual void DoPaint(CDCHandle& dc);
	virtual void GetMinSize(CSize& size);
	
	BOOL GetShowCloseButton();
	void GetTabButtonMargins(int& leftMarging, int& topMarging, int& rightMargin, int& bottomMarging);
	UINT GetTabsCount() const;
	
private:
	int m_current;							// Current Select tab
	CAtlList <CWidget *> vWidgets;			// Widgets (tabs)
	CWidget *m_lastOver;					// Last widget that mouse was over
	CWidget *m_lastClick;					// Last widget that mouse clicked
	int m_lines;							// Number of current tab lines
	int m_maxLines;							// Max number of tab lines
	int m_minWidth;							// Min width to a tab
	int m_maxTabs;							// Max number of tabs
	int m_blinkingItems;
	int m_timer;
	//CRect m_rectClose;
	BOOL m_blinking;
	IUOLMessengerImagePtr	m_pOverImage;	
	IUOLMessengerImagePtr	m_pClikedImage;	
	IUOLMessengerImagePtr	m_pNormalImage;	
	IUOLMessengerImagePtr	m_pSelectedImage;	
	IUOLMessengerImagePtr	m_pCloseTabImage;
	IUOLMessengerImagePtr	m_pBlinkingImage;
	CFontElementPtr			m_pFont;
	CFontElementPtr			m_pFontSelected;
	CTabViewButtonElementPtr m_pTabButton;
	CAtlList<IUOLMessengerTabViewCtrlObserver2*> m_listObservers;
	
	int						m_tabViewType;
	BOOL					m_showCloseButton;
	BOOL					m_bAutoCalculateWidths;
	
	int						m_buttonSpace;
	int						m_buttonLeftMargin;
	int						m_buttonTopMargin;
	int						m_buttonRightMargin;
	int						m_buttonBottomMargin;
	
	CStringGUID				m_strGUID;
	
	int						m_nTotalTabsWidth;
	BOOL					m_bTabsChanged;
	BOOL					m_bHover;

	void RecalcTabs(CDCHandle& dc);
	void OnDestroy(void);
	void OnTimer(UINT nTimerId, TIMERPROC tp);
	void DrawTabs(CDCHandle& dc);
	void NotifyAll(void (IUOLMessengerTabViewCtrlObserver2::*pfnHandler)(IUOLMessengerTabViewCtrl*, 
			IUOLMessengerConversationPtr, int), IUOLMessengerConversationPtr pConversation, int nTabIndex);
	void NotifyAll(void (IUOLMessengerTabViewCtrlObserver2::*pfnHandler)(IUOLMessengerTabViewCtrl*, 
			IUOLMessengerConversationPtr, CPoint&, HWND), IUOLMessengerConversationPtr pConversation, CPoint& pt, HWND wHandle);
	void NotifyAll(void (IUOLMessengerTabViewCtrlObserver2::*pfnHandler) (void));

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);	
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnCreate(LPCREATESTRUCT lpCs);
	
	int		CalculateTabsTotalWidth();
	
public:
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual void OnElementBuilt();
	void SetBlinking(IUOLMessengerConversationPtr pConversation);
	void SetActiveConversation(IUOLMessengerConversationPtr pConversation);
	BOOL AddTab(IUOLMessengerConversationPtr pConversation,
				const CString& strCaption,
				CWidget::tabState state,
				BOOL bInformTabView = FALSE);		// Add a new tab
	BOOL RemoveTab(IUOLMessengerConversationPtr pConversation);	// Remove tab

	void SetActiveTab(int nIndex);
	void SetMaxTabsLines(int max) {	m_maxLines = max;}	// m_maxlines
	int  GetMaxTabsLines() { return m_maxLines;}		
	void SetMaxTabs(int max) { m_maxTabs = max;}			// m_maxTabs
	int  GetMaxTabs()	{	return m_maxTabs;}
	int GetTabCount(CWidget::tabState state);

    void SetCaption(IUOLMessengerConversationPtr pConversation, const CString& strCaption);
	
	void RegisterObserver(IUOLMessengerTabViewCtrlObserver2* pObserver);
	void UnregisterObserver(IUOLMessengerTabViewCtrlObserver2* pObserver);
	
	virtual void SetTabViewType(int tabViewType);
	virtual void ShowCloseButton(BOOL bShow);
	virtual void SetInterButtonSpace(int buttonSpace);
	virtual void SetTabButtonMargins(int leftMarging, int topMarging, int rightMargin, int bottomMarging);
	virtual void AutoCalculateTabsWidths(BOOL bCalculate);
	
	virtual int  CreateTabButton(CString tabButtonName);
	virtual BOOL SetTabButtonImage(int nTabIndex, HMODULE hModule, DWORD dwResourceID);
	virtual BOOL RemoveTabButton(int nTabIndex);
};

MAKEAUTOPTR(CTabView);
