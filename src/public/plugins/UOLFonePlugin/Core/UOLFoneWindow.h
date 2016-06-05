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

#include <interfaces/IUOLMessengerPluginCustomTab.h>
#include "../UOLFoneTooltipProvider.h"

#include "WTLWndComponentsOrganizer.h"
#include "UOLFoneChildWindowContainerView.h"
#include "UOLFoneOrganizedColoredPanel.h"
#include "UOLFoneStatic.h"
#include "UOLFoneMultiStateButton.h"
#include "WebBrowserWnd.h"
#include "../SeparatorWnd.h"




/////////////////////////////////////////////////////////////////////
// CUOLFoneToolbarPanel
/////////////////////////////////////////////////////////////////////


class CUOLFoneToolbarPanel : public CUOLFoneOrganizedColoredPanel
{
public:
	
	CUOLFoneToolbarPanel(void);
	~CUOLFoneToolbarPanel(void);
	void OnLostCall();
	void OnHistoryAccessed();
	BOOL IsAlertingLostCall();
	void ShowSACButton(BOOL bShow);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneToolbarPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
private:
	
	WTL::CButton		m_btnCallHistory;
	WTL::CButton		m_btnPrices;
	WTL::CButton		m_btnConfig;
	WTL::CButton		m_btnMyAccount;
	WTL::CButton		m_btnCallSAC;
	
	CUOLFoneMultiStateButton	m_multiStateBtnCallHistory;
	CUOLFoneMultiStateButton	m_multiStateBtnPrices;
	CUOLFoneMultiStateButton	m_multiStateBtnConfig;
	CUOLFoneMultiStateButton	m_multiStateBtnMyAccount;
	CUOLFoneMultiStateButton	m_multiStateBtnCallSAC;

	BOOL isAlertingLostCall;
};



/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountInfoLine
/////////////////////////////////////////////////////////////////////


class CUOLFoneAccountInfoLine : public CUOLFoneOrganizedColoredPanel
{
public:
	
	CUOLFoneAccountInfoLine(void);
	~CUOLFoneAccountInfoLine(void);
	
	void InitializeLabels(CString strLabelText, CString strLabelValue, UINT nLineButtonId, UINT nLineButtonImageIDD);
	void UpdateLabels(CString strLabelText, CString strLabelValue);
	void ShowButtonLine(BOOL bShow);
	void ShowBoldFont(BOOL bShowBold);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneAccountInfoLine)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
private:
	WTL::CStatic		m_labelText;
	WTL::CStatic		m_middleFiller;
	WTL::CStatic		m_labelValue;
		
	WTL::CFont			m_textFont;
	WTL::CFont			m_textFontBold;

	WTL::CFont			m_valueFont;
	WTL::CFont			m_valueFontBold;

	WTL::CButton		m_lineButton;
	UINT				m_nLineButtonId;
	UINT				m_nLineButtonImageIDD;

	CUOLFoneMultiStateButton	m_multiStateLineButton;
	
	POSITION			m_labelLinePosition;
	POSITION			m_buttonLinePosition;

	CString				m_strLabelText;
	CString				m_strLabelValue;
};




/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountInfoPanel
/////////////////////////////////////////////////////////////////////


class CUOLFoneAccountInfoPanel : public CUOLFoneOrganizedColoredPanel
{
public:
	
	CUOLFoneAccountInfoPanel(void);
	~CUOLFoneAccountInfoPanel(void);
	
	void UpdateLabels(double dUserBalance, int nPlanSecondsBalance, CString strPlanName);
	void ShowBuyVoipButton(BOOL bShow);
	void UpdateLayout();
	
	void StartBlink();
	void StopBlink();
	BOOL IsBlinking();

	void SetSessionUsingCredits(BOOL bUsing);
	void SetCallInPlace(BOOL bCallInPlace);
	void SetHasVoipOutPlan(BOOL bHasPlan);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneAccountInfoPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
private:
	WTL::CButton		m_btnBuyVoip;
	
	CUOLFoneMultiStateButton	m_multiStateBtnBuyVoip;
	
	COLORREF			m_backgroundColor;
	
	BOOL						m_destroyingWindow;
	CComAutoCriticalSection		m_destroyWindowLock;
	
	UINT_PTR			m_blinkTimer;
	
	BOOL				m_bIsBalanceVisible;
	BOOL				m_bIsPlanVisible;

	POSITION			m_BuyVoipButtonLine;
	POSITION			m_SeparatorLine;
	POSITION			m_PlanLine;
	POSITION			m_CreditLine;

	BOOL				m_bShowBuyVoipButton;

	double				m_userBalance;
	int					m_nPlanRemainingSeconds;

	BOOL				m_bSessionUsingCredits;
	BOOL				m_bIsCallInPlace;
	BOOL				m_bHasVoipOutPlan;

	CSeparatorWnd			m_SeparatorWnd;
	CUOLFoneAccountInfoLine	m_creditLinePanel;
	CUOLFoneAccountInfoLine	m_planLinePanel;
};




/////////////////////////////////////////////////////////////////////
// CUOLFoneStatusPanel
/////////////////////////////////////////////////////////////////////


class CUOLFoneStatusPanel : public CUOLFoneOrganizedColoredPanel
{
public:
	
	CUOLFoneStatusPanel(void);
	~CUOLFoneStatusPanel(void);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneStatusPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	void UpdateCallDuration(int callDuration, double costPerMinute, BOOL bSessionUsingCredits, int nPlanSeconds);
	
private:
	
	WTL::CStatic		m_durationLabel;
	WTL::CStatic		m_durationFiller;
	WTL::CStatic		m_duration;
	WTL::CStatic		m_costLabel;
	WTL::CStatic		m_costFiller;
	WTL::CStatic		m_cost;
	
	WTL::CFont			m_normalFont;
	WTL::CFont			m_boldFont;
	
	COLORREF			m_backgroundColor;
};

/////////////////////////////////////////////////////////////////////
// CUOLFonePromoAdPanel
/////////////////////////////////////////////////////////////////////


class CUOLFonePromoAdPanel : public CUOLFoneOrganizedColoredPanel,
							 public IWebBrowserWindowHolder
{
public:
	
	DECLARE_WND_CLASS("CUOLFonePromoAdPanel");

	CUOLFonePromoAdPanel(void);
	~CUOLFonePromoAdPanel(void);

	BOOL SetUrl(const CString& strUrl);

	BOOL ShouldCancelNavigation(CString strURL);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFonePromoAdPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
private:

	CWebBrowserWindow		m_promoAdWnd;
};


/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow
/////////////////////////////////////////////////////////////////////


class CUOLFoneWindow;

typedef CWinTraits<WS_CHILD | WS_CLIPCHILDREN, WS_EX_TOPMOST> CUOLFoneWindowTraits;


class CUOLFoneWindow : 
		public CWindowImpl<CUOLFoneWindow, CWindow, CUOLFoneWindowTraits>, 
		public IUOLMessengerPluginCustomTab2,
		public COrganizerWindow
{
public:
	enum
	{
		ID_BUTTON_HOME_UOL = 0x2723,
		ID_BUTTON_PRICES,
		ID_BUTTON_BUY_CREDIT,
		ID_BUTTON_MY_ACCOUNT,
		ID_BUTTON_HISTORICO,
		ID_BUTTON_PROCEED_CALL,
		ID_BUTTON_HANG_CALL,
		ID_BUTTON_PREFERENCES,
		ID_BUTTON_CALL_HISTORY,
		ID_BUTTON_CALL_SAC,
		ID_BUTTON_BUY_CREDIT_LINE,
		ID_BUTTON_BUY_PLAN_LINE
	};
	
	DECLARE_WND_CLASS(NULL)
	
	CUOLFoneWindow(const CString& strTabName, const CString& strPluginInternalName);
	~CUOLFoneWindow();
	
	CString GetTabName() const;
	CString GetPluginInternalName() const;
	void GetMinSize(CSize& size);
	HWND CreateTab(HWND hWndParent, const CRect& rectTab, DWORD dwWindowStyle = WS_CHILDWINDOW);
	void DestroyTab(HWND hWndTab);
	HWND GetTabHWND();
	void SetTabButtonIndex(int nTabIndex);
	int  GetTabButtonIndex();
	IUOLMessengerTooltipProviderPtr GetTooltipProvider() const;
	
	void UpdateWindowInterface(int showingWindowID);
	void UpdateAccountStats(CUOLFoneAccountPtr pAccount);
	void UpdatePhoneCallStats(CPhoneCallPtr pPhoneCall);
	void OnLostCall();
	void ShowSACButton(BOOL bShow);
	void UpdateBalancePanel(CUOLFoneAccountPtr pAccount);

#ifdef  _TAB_DEMO_MODE
	void CallNextWindow();
#endif
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneWindow32)
		CHAIN_MSG_MAP(COrganizerWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_RANGE_HANDLER_EX(ID_BUTTON_HOME_UOL, ID_BUTTON_BUY_PLAN_LINE, OnButtonClicked)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	LRESULT OnCtlColorStatic(HDC hDc, HWND);
	
	void StartCallControl();
	void StopCallControl();
	
private:
	
	// Plugin elements.
	CString		m_strTabName;
	CString		m_strPluginInternalName;
	CUOLFoneTooltipProviderPtr	m_pTooltipProvider;
	
	// Interface elements.
	CUOLFoneToolbarPanel		m_toolbarPanel;
	CUOLFoneAccountInfoPanel	m_accountInfoPanel;
	CUOLFonePromoAdPanel		m_promoPanel;
	
	CUOLFoneChildWindowContainerView	m_childContainerView;
	
	CUOLFoneMultiStateButton	m_multiStateProceedCall;
	CUOLFoneMultiStateButton	m_multiStateHangCall;
	WTL::CButton				m_proceedCall;
	WTL::CButton				m_hangCall;
	
	CUOLFoneStatic				m_bottomFiller;
	
	CUOLFoneStatic				m_bottomSeparator;
	
	CUOLFoneStatusPanel			m_statusPanel;
	
	// Control elements.
	BOOL						m_destroyingWindow;
	CComAutoCriticalSection		m_destroyWindowLock;
	
	UINT_PTR				m_callInPlaceTimer;
	int						m_timerHits;
	double					m_userBalance;
	double					m_costPerMinute;

	BOOL					m_isCallingSac;
	
	POSITION				m_childContainerViewPos;
	BOOL					m_hasHiddenAll;
	
	POSITION				m_pluginToolBarPos;
	POSITION				m_panelAccountInfoPos;
	POSITION				m_promoPanelPos;
	
	int						m_nTabIndex;
	int						m_showingWindowID;
	
	BOOL					m_bHasPendingUpdate;
	BOOL					m_bIsSessionUsingCredits;
	BOOL					m_bForceBlink;

	CString					m_strPlanName;
	int						m_nPlanRemainingSeconds;
	int						m_nPlanSeconds;
};

MAKEAUTOPTR(CUOLFoneWindow);

