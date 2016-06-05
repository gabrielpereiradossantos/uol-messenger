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
 

#include "StdAfx.h"
#include ".\UOLFoneWindow.h"

#include "../FacadeWrapper.h"
#include "../Resource.h"
#include "../UOLFonePluginTheme.h"
#include "./UOLFoneController.h"
#include <math.h>





/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow positioning defines
/////////////////////////////////////////////////////////////////////


#define		DEFAULT_SHIFT						0

#define		LEFT_SHIFT							DEFAULT_SHIFT
#define		TOP_SHIFT							DEFAULT_SHIFT
#define		RIGHT_SHIFT							DEFAULT_SHIFT
#define		BOTTOM_SHIFT						DEFAULT_SHIFT

#define		INNER_COMPONENT_DEFAULT_HEIGHT				25
#define		INNER_COMPONENTS_HORIZONTAL_SEPARATION		10
#define		INNER_COMPONENTS_VERTICAL_SEPARATION		10

#define		VERTICAL_BLOCK_SEPARATION					1

#define		PREFERENCES_BUTTON_WIDTH					110

#define		CALL_IN_PLACE_TIMER_ID				1234
#define		BLINK_TIMER_ID				1235

#define		CALL_IN_PLACE_UPDATE_TIMEOUT		1000
#define		BLINK_TIMER_TIMEOUT					500
#define		ONE_MINUTE				60

#define		LINE_HEIGHT				17
#define		LINE_FULL_HEIGHT		35

#define		COLOR_BLINK_ON			RGB(0xDA, 0xE7, 0xEE)
#define		COLOR_BLINK_OFF			RGB(0xEC, 0xF5, 0xFA)


/////////////////////////////////////////////////////////////////////
// CUOLFoneToolbarPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneToolbarPanel::CUOLFoneToolbarPanel(void)
{
}


CUOLFoneToolbarPanel::~CUOLFoneToolbarPanel(void)
{
}


LRESULT CUOLFoneToolbarPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls ...
	//
	isAlertingLostCall = FALSE;

	CString strToolTipText;
	
	SetBackgroundColor(CUOLFonePluginTheme::GetInstance()->GetToolbarColor());
	Initialize(1, 2, 1, 2);
	
	m_btnCallSAC.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, CUOLFoneWindow::ID_BUTTON_CALL_SAC);
	m_multiStateBtnCallSAC.SubclassWindow(m_btnCallSAC.m_hWnd);
	m_multiStateBtnCallSAC.SetMultiStateBitmap(IDB_TOOLBAR_BTN_CALL_SAC);
	strToolTipText.LoadString(IDS_TOOLBAR_BTN_LABEL_SAC);
	m_multiStateBtnCallSAC.SetToolTipText(strToolTipText);
	m_multiStateBtnCallSAC.SetToolTipDelays(5000, 1000, 0);
	
	m_btnMyAccount.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, CUOLFoneWindow::ID_BUTTON_MY_ACCOUNT);
	m_multiStateBtnMyAccount.SubclassWindow(m_btnMyAccount.m_hWnd);
	m_multiStateBtnMyAccount.SetMultiStateBitmap(IDB_TOOLBAR_BTN_MY_ACCOUNT);
	strToolTipText.LoadString(IDS_TOOLBAR_BTN_LABEL_MY_ACCOUNT);
	m_multiStateBtnMyAccount.SetToolTipText(strToolTipText);
	m_multiStateBtnMyAccount.SetToolTipDelays(5000, 400, 0);
	
	m_btnPrices.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, CUOLFoneWindow::ID_BUTTON_PRICES);
	m_multiStateBtnPrices.SubclassWindow(m_btnPrices.m_hWnd);
	m_multiStateBtnPrices.SetMultiStateBitmap(IDB_TOOLBAR_BTN_PRICES);
	strToolTipText.LoadString(IDS_TOOLBAR_BTN_LABEL_PRICES);
	m_multiStateBtnPrices.SetToolTipText(strToolTipText);
	m_multiStateBtnPrices.SetToolTipDelays(5000, 400, 0);

	m_btnConfig.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, CUOLFoneWindow::ID_BUTTON_PREFERENCES);
	m_multiStateBtnConfig.SubclassWindow(m_btnConfig.m_hWnd);
	m_multiStateBtnConfig.SetMultiStateBitmap(IDB_TOOLBAR_BTN_CONFIG);
	strToolTipText.LoadString(IDS_TOOLBAR_BTN_LABEL_CONFIG);
	m_multiStateBtnConfig.SetToolTipText(strToolTipText);
	m_multiStateBtnConfig.SetToolTipDelays(5000, 400, 0);

	m_btnCallHistory.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, CUOLFoneWindow::ID_BUTTON_CALL_HISTORY);
	m_multiStateBtnCallHistory.SubclassWindow(m_btnCallHistory.m_hWnd);
	m_multiStateBtnCallHistory.SetMultiStateBitmap(IDB_TOOLBAR_BTN_CALL_HISTORY);
	strToolTipText.LoadString(IDS_TOOLBAR_BTN_LABEL_CALL_HISTORY);
	m_multiStateBtnCallHistory.SetToolTipText(strToolTipText);
	m_multiStateBtnCallHistory.SetToolTipDelays(5000, 400, 0);
	
	ATLASSERT(m_btnMyAccount.IsWindow());
	ATLASSERT(m_btnPrices.IsWindow());
	ATLASSERT(m_btnConfig.IsWindow());
	ATLASSERT(m_btnCallHistory.IsWindow());
	ATLASSERT(m_btnCallSAC.IsWindow());
	
	// Building layout ...
	
	// Build line 1.
	AddLine(2);
	AddComponent(&m_btnMyAccount, 
			m_btnMyAccount.GetWndClassName(), 
			m_multiStateBtnMyAccount.GetWidth(), 0, 
			m_multiStateBtnMyAccount.GetHeight());
	AddComponent(&m_btnPrices, 
			m_btnPrices.GetWndClassName(), 
			m_multiStateBtnPrices.GetWidth(), 0, 
			m_multiStateBtnPrices.GetHeight());
	AddComponent(&m_btnConfig, 
			m_btnConfig.GetWndClassName(), 
			m_multiStateBtnConfig.GetWidth(), 0, 
			m_multiStateBtnConfig.GetHeight());
	AddComponent(&m_btnCallHistory,
			m_btnCallHistory.GetWndClassName(), 
			m_multiStateBtnCallHistory.GetWidth(), 0, 
			m_multiStateBtnCallHistory.GetHeight());
	
	if (!(CFacadeWrapper::GetInstance()->GetSACnumber()).IsEmpty())
	{
		AddComponent(&m_btnCallSAC, 
			m_btnCallSAC.GetWndClassName(), 
			m_multiStateBtnCallSAC.GetWidth(), 0, 
			m_multiStateBtnCallSAC.GetHeight());
		
		CFacadeWrapper::GetInstance()->ShowSACButton();
	}

	return 0L;
}


LRESULT CUOLFoneToolbarPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_btnMyAccount.DestroyWindow();
	m_btnPrices.DestroyWindow();
	m_btnConfig.DestroyWindow();
	m_btnCallHistory.DestroyWindow();
	m_btnCallSAC.DestroyWindow();
	
	return 0L;
}

void CUOLFoneToolbarPanel::OnLostCall()
{
	if (!isAlertingLostCall)
	{
		CString strToolTipText;
		m_multiStateBtnCallHistory.SetMultiStateBitmap(IDB_TOOLBAR_BTN_CALL_HISTORY_ALERT);
		strToolTipText.LoadString(IDS_TOOLBAR_BTN_LABEL_LIG_PERDIDAS);
		m_multiStateBtnCallHistory.SetToolTipText(strToolTipText);
		isAlertingLostCall = TRUE;
	}
}
BOOL CUOLFoneToolbarPanel::IsAlertingLostCall()
{
	return isAlertingLostCall;
}

void CUOLFoneToolbarPanel::OnHistoryAccessed()
{
	if (isAlertingLostCall)
	{
		m_multiStateBtnCallHistory.SetMultiStateBitmap(IDB_TOOLBAR_BTN_CALL_HISTORY);
		isAlertingLostCall = FALSE;
	}
}

void CUOLFoneToolbarPanel::ShowSACButton(BOOL bShow)
{
	m_btnCallSAC.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}


/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountInfoLine
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountInfoLine::CUOLFoneAccountInfoLine()
{
}


CUOLFoneAccountInfoLine::~CUOLFoneAccountInfoLine(void)
{
}


void CUOLFoneAccountInfoLine::InitializeLabels(CString strLabelText, CString strLabelValue, UINT nLineButtonId, UINT nLineButtonImageIDD)
{
	m_nLineButtonId = nLineButtonId;
	m_nLineButtonImageIDD = nLineButtonImageIDD;
	m_strLabelText = strLabelText;
	m_strLabelValue = strLabelValue;
}


LRESULT CUOLFoneAccountInfoLine::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// Check if it was initialized
	ATLASSERT(m_nLineButtonId);
	ATLASSERT(m_nLineButtonImageIDD);

	CString labelStr;
	
	//
	// Creating controls ...
	//

	SetTextColor(RGB(0x46, 0x49, 0x4b));

	DWORD dwStyle = WS_CHILDWINDOW | WS_VISIBLE;
	
	m_textFont.CreateFont(-11, 0, 1, 1, FW_NORMAL, FALSE, FALSE, FALSE, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, FF_DONTCARE, "Tahoma");
	m_textFontBold.CreateFont(-11, 0, 1, 1, FW_BOLD, FALSE, FALSE, FALSE, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, FF_DONTCARE, "Tahoma");

	m_valueFont.CreateFont(-9, 0, 1, 1, FW_NORMAL, FALSE, FALSE, FALSE, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, FF_DONTCARE, "Tahoma");
	m_valueFontBold.CreateFont(-9, 0, 1, 1, FW_BOLD, FALSE, FALSE, FALSE, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, FF_DONTCARE, "Tahoma");
	
	Initialize(10, 0, 10, 0);
	
	// Label Line
	m_labelText.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, dwStyle);
	m_labelText.SetFont(m_textFont);
	
	m_middleFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, dwStyle);
	
	m_labelValue.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, dwStyle);
	m_labelValue.SetFont(m_valueFont);
	
	// Button Line
	m_lineButton.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, m_nLineButtonId);
	m_multiStateLineButton.SubclassWindow(m_lineButton.m_hWnd);
	m_multiStateLineButton.SetMultiStateBitmap(m_nLineButtonImageIDD);
	m_multiStateLineButton.SetButtonIsLink(TRUE);
	//CString strToolTipText;
	//strToolTipText.LoadString(IDS_BTN_BUY_CREDIT_TOOLTIP);
	//m_multiStateLineButton.SetToolTipText(strToolTipText);
	//m_multiStateLineButton.SetToolTipDelays(5000, 400, 0);

	ATLASSERT(m_labelText.IsWindow());
	ATLASSERT(m_middleFiller.IsWindow());
	ATLASSERT(m_labelValue.IsWindow());
	ATLASSERT(m_lineButton.IsWindow());
		
	// Building layout ...
	// Build text line
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT,
		CWTLWndComponentsOrganizer::LINE_VORIENTATION_CENTERED);
	m_labelLinePosition = GetLastAddedLinePosition();
	AddComponent(&m_labelText, m_labelText.GetWndClassName(), 0, 0, LINE_HEIGHT);
	AddComponent(&m_middleFiller, m_middleFiller.GetWndClassName(), 5, 0, LINE_HEIGHT, TRUE);
	AddComponent(&m_labelValue, m_labelValue.GetWndClassName(), 0, 0, LINE_HEIGHT);
	
	// Build button line
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED, 
		CWTLWndComponentsOrganizer::LINE_VORIENTATION_CENTERED);
	m_buttonLinePosition = GetLastAddedLinePosition();
	AddComponent(&m_lineButton, m_lineButton.GetWndClassName(), 
			m_multiStateLineButton.GetWidth(), 
			m_multiStateLineButton.GetHeight(), 
			m_multiStateLineButton.GetHeight());

	return 0L;
}


LRESULT CUOLFoneAccountInfoLine::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_labelText.DestroyWindow();
	m_middleFiller.DestroyWindow();
	m_labelValue.DestroyWindow();
	m_lineButton.DestroyWindow();
	
	m_textFont.DeleteObject();
	m_textFontBold.DeleteObject();
	m_valueFont.DeleteObject();
	m_valueFontBold.DeleteObject();
	
	return 0L;
}


void CUOLFoneAccountInfoLine::UpdateLabels(CString strLabelText, CString strLabelValue)
{
	m_labelText.SetWindowText(strLabelText);
	m_labelValue.SetWindowText(strLabelValue);
	
	RecalculateLinesHeightsAndWidths();
	OrganizeComponents();
}


void CUOLFoneAccountInfoLine::ShowButtonLine(BOOL bShow)
{
	ShowLineComponents(bShow, m_buttonLinePosition);
	ShowLineComponents(!bShow, m_labelLinePosition);

	RecalculateLinesHeightsAndWidths();
	OrganizeComponents(TRUE);
}


void CUOLFoneAccountInfoLine::ShowBoldFont(BOOL bShowBold)
{
	m_labelText.SetFont(bShowBold ? m_textFontBold : m_textFont);
	m_labelValue.SetFont(bShowBold ? m_valueFontBold : m_valueFont);
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountInfoPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountInfoPanel::CUOLFoneAccountInfoPanel(void) : 
		m_destroyingWindow(FALSE),
		m_blinkTimer(NULL),
		m_bIsBalanceVisible(TRUE),
		m_bShowBuyVoipButton(TRUE),
		m_userBalance(0.0),
		m_nPlanRemainingSeconds(0),
		m_bIsPlanVisible(TRUE),
		m_bIsCallInPlace(FALSE),
		m_SeparatorWnd(IDB_LINE_SEPARATOR),
		m_bHasVoipOutPlan(FALSE)
{
	m_backgroundColor = COLOR_BLINK_OFF;
}


CUOLFoneAccountInfoPanel::~CUOLFoneAccountInfoPanel(void)
{
}


void CUOLFoneAccountInfoPanel::StartBlink()
{
	if (m_blinkTimer == NULL)
	{
		// Start blink timer
		m_blinkTimer = (UINT_PTR) SetTimer(BLINK_TIMER_ID, BLINK_TIMER_TIMEOUT);
		ATLASSERT(m_blinkTimer != NULL);
	}
}


void CUOLFoneAccountInfoPanel::StopBlink()
{
	if (m_blinkTimer != NULL)
	{
		if ( KillTimer(m_blinkTimer) )
		{
			m_blinkTimer = NULL;
		}
	}
	
	m_creditLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);
	m_creditLinePanel.Invalidate(TRUE);

	m_planLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);
	m_planLinePanel.Invalidate(TRUE);	
	
	m_bIsBalanceVisible = TRUE;
	m_bIsPlanVisible = TRUE;
}


BOOL CUOLFoneAccountInfoPanel::IsBlinking()
{
	return (NULL != m_blinkTimer);
}



LRESULT CUOLFoneAccountInfoPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString labelStr;
	CString strToolTipText;

	DWORD dwStyle = WS_CHILDWINDOW | WS_VISIBLE;
	
	SetBackgroundColor(m_backgroundColor);
	Initialize(0, 6, 0, 5);

	//
	// Creating controls ...
	//

	// Credit Line
	labelStr.LoadString(IDS_LABEL_BALANCE);
	m_creditLinePanel.InitializeLabels(labelStr, "0", 
		CUOLFoneWindow::ID_BUTTON_BUY_CREDIT_LINE, IDB_BTN_BUY_CREDITS_LINE);
	m_creditLinePanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, dwStyle);
	m_creditLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);

	// Plan Line
	labelStr.LoadString(IDS_LABEL_PLAN);
	m_planLinePanel.InitializeLabels(labelStr, "0", 
		CUOLFoneWindow::ID_BUTTON_BUY_PLAN_LINE, IDB_BTN_BUY_PLAN_LINE);
	m_planLinePanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, dwStyle);
	m_planLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);

	// Buy UOL Voip button
	m_btnBuyVoip.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, CUOLFoneWindow::ID_BUTTON_BUY_CREDIT);
	m_multiStateBtnBuyVoip.SubclassWindow(m_btnBuyVoip.m_hWnd);
	m_multiStateBtnBuyVoip.SetMultiStateBitmap(IDB_BTN_BUY_VOIP);
	strToolTipText.LoadString(IDS_BTN_BUY_CREDIT_TOOLTIP);
	m_multiStateBtnBuyVoip.SetToolTipText(strToolTipText);
	m_multiStateBtnBuyVoip.SetToolTipDelays(5000, 400, 0);

	m_SeparatorWnd.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, dwStyle);
		
	ATLASSERT(m_planLinePanel.IsWindow());
	ATLASSERT(m_creditLinePanel.IsWindow());
	ATLASSERT(m_btnBuyVoip.IsWindow());
	ATLASSERT(m_SeparatorWnd.IsWindow());
	
	// Building layout ...
	// Build Plan line
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT,
		CWTLWndComponentsOrganizer::LINE_VORIENTATION_TOP);
	m_PlanLine = GetLastAddedLinePosition();
	AddComponent(&m_planLinePanel, m_planLinePanel.GetWndClassName(), 0, 0, LINE_HEIGHT, TRUE);
	
	// Build Separator line
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT,
		CWTLWndComponentsOrganizer::LINE_VORIENTATION_TOP);
	m_SeparatorLine = GetLastAddedLinePosition();
	AddComponent(&m_SeparatorWnd, m_SeparatorWnd.GetWndClassName(), 0, 0, 1, TRUE);
	
	// Build Credit line.
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT,
		CWTLWndComponentsOrganizer::LINE_VORIENTATION_TOP);
	m_CreditLine = GetLastAddedLinePosition();
	AddComponent(&m_creditLinePanel, m_creditLinePanel.GetWndClassName(), 0, 0, LINE_HEIGHT, TRUE);
	
	// Build Buy VOIP Button line.
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED, 
		CWTLWndComponentsOrganizer::LINE_VORIENTATION_CENTERED);
	m_BuyVoipButtonLine = GetLastAddedLinePosition();
	AddComponent(&m_btnBuyVoip, m_btnBuyVoip.GetWndClassName(), 
			m_multiStateBtnBuyVoip.GetWidth(), 
			m_multiStateBtnBuyVoip.GetHeight());
	ShowLineComponents(FALSE, m_BuyVoipButtonLine);

	UpdateLayout();
	return 0L;
}


LRESULT CUOLFoneAccountInfoPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	m_destroyingWindow = TRUE;
	
	// Disable user balance blink timer
	if (m_blinkTimer != NULL)
	{
		if ( KillTimer(m_blinkTimer) )
		{
			m_blinkTimer = NULL;
		}
	}
	
	m_planLinePanel.DestroyWindow();
	m_creditLinePanel.DestroyWindow();
	m_SeparatorWnd.DestroyWindow();
	m_btnBuyVoip.DestroyWindow();
	
	return 0L;
}


LRESULT CUOLFoneAccountInfoPanel::OnTimer(UINT /*uMsg*/, WPARAM wTimerID, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	if (!m_destroyingWindow)
	{
		if (m_bSessionUsingCredits) 
		{
			if (m_bIsBalanceVisible == TRUE)
			{
				m_creditLinePanel.SetBackgroundColor(COLOR_BLINK_ON);
				m_bIsBalanceVisible = FALSE;
			}
			else 
			{
				m_creditLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);
				m_bIsBalanceVisible = TRUE;
			}

			m_planLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);
			m_planLinePanel.Invalidate(TRUE);
			m_creditLinePanel.Invalidate(TRUE);
		}
		else
		{
			if (m_bIsPlanVisible == TRUE)
			{
				m_planLinePanel.SetBackgroundColor(COLOR_BLINK_ON);
				m_bIsPlanVisible = FALSE;
			}
			else 
			{
				m_planLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);
				m_bIsPlanVisible = TRUE;
			}

			m_creditLinePanel.SetBackgroundColor(COLOR_BLINK_OFF);
			m_creditLinePanel.Invalidate(TRUE);
			m_planLinePanel.Invalidate(TRUE);
		}
	}
	
	return 0L;
}


void CUOLFoneAccountInfoPanel::UpdateLabels(double userBalance, int nPlanRemainingSeconds, CString strPlanName)
{
	// Update credit label
	m_userBalance = userBalance;

	CString	strBalanceValue;
	strBalanceValue.Format("R$ %.2f", userBalance);

	CString strBalanceText;
	strBalanceText.LoadString(IDS_LABEL_BALANCE);

	m_creditLinePanel.UpdateLabels(strBalanceText, strBalanceValue);
	
	// Update plan label
	m_nPlanRemainingSeconds = nPlanRemainingSeconds;
	
	int nMinutes = (int)floor((float)nPlanRemainingSeconds / 60.0);
	CString	strPlanValue;
	strPlanValue.Format("%d min", nMinutes);

	m_planLinePanel.UpdateLabels(strPlanName, strPlanValue);
	
	RecalculateLinesHeightsAndWidths();
	OrganizeComponents();
}


void CUOLFoneAccountInfoPanel::ShowBuyVoipButton(BOOL bShow)
{
	BOOL bHasCredits = (m_userBalance > 0.0);

	m_bShowBuyVoipButton = bShow;

	BOOL bShowBuyVoipBtn = m_bShowBuyVoipButton && !bHasCredits && !m_bHasVoipOutPlan;

	if (bShowBuyVoipBtn)
	{
		Initialize(0, 6, 0, 5);
	}
	else
	{
		Initialize(0, 0, 0, 0);
	}
}


void CUOLFoneAccountInfoPanel::UpdateLayout()
{
	BOOL bHasCredits = (m_userBalance > 0.0);
	
	BOOL bShowBuyVoipBtn = m_bShowBuyVoipButton && !bHasCredits && !m_bHasVoipOutPlan;

	//ATLTRACE(_T("%s - bShow = %d, m_userBalance = %.2lf\n"), __FUNCTION__, bShow, m_userBalance);

	m_creditLinePanel.ShowButtonLine(!bHasCredits);
	m_planLinePanel.ShowButtonLine(!m_bHasVoipOutPlan);

	ShowLineComponents(!bShowBuyVoipBtn, m_PlanLine);
	ShowLineComponents(!bShowBuyVoipBtn, m_CreditLine);
	ShowLineComponents(!bShowBuyVoipBtn, m_SeparatorLine);
	ShowLineComponents(bShowBuyVoipBtn, m_BuyVoipButtonLine);

	RecalculateLinesHeightsAndWidths();
	OrganizeComponents(TRUE);
}


void CUOLFoneAccountInfoPanel::SetSessionUsingCredits(BOOL bUsing)
{
	m_bSessionUsingCredits = bUsing;

	if (m_bIsCallInPlace)
	{
		m_creditLinePanel.ShowBoldFont(m_bSessionUsingCredits);
		m_planLinePanel.ShowBoldFont(!m_bSessionUsingCredits);
	}
	else
	{
		m_creditLinePanel.ShowBoldFont(FALSE);
		m_planLinePanel.ShowBoldFont(FALSE);
	}

	UpdateLayout();
}


void CUOLFoneAccountInfoPanel::SetCallInPlace(BOOL bCallInPlace)
{
	if (m_bIsCallInPlace != bCallInPlace) 
	{
		m_bIsCallInPlace = bCallInPlace;
	
		UpdateLayout();
	}
}


void CUOLFoneAccountInfoPanel::SetHasVoipOutPlan(BOOL bHasPlan)
{
	m_bHasVoipOutPlan = bHasPlan;
}



/////////////////////////////////////////////////////////////////////
// CUOLFoneStatusPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneStatusPanel::CUOLFoneStatusPanel(void)
{
}


CUOLFoneStatusPanel::~CUOLFoneStatusPanel(void)
{
}


LRESULT CUOLFoneStatusPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString labelStr;
	
	//
	// Creating controls ...
	//
	
	m_normalFont.CreatePointFont(80, "Tahoma");
	m_boldFont.CreatePointFont(80, "Tahoma", 0, true);
	
	Initialize(10, 3, 10, 3);
	
	labelStr.LoadString(IDS_LABEL_DURATION);
	m_durationLabel.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_durationLabel.SetFont(m_normalFont);
	
	m_durationFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	labelStr = "00:00:00";
	m_duration.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_duration.SetFont(m_boldFont);
	
	labelStr.LoadString(IDS_LABEL_COST);
	m_costLabel.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_costLabel.SetFont(m_normalFont);
	
	m_costFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	labelStr.Format("(%s %.2f%s)", "R$", 0.0, "/min");
	m_cost.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_cost.SetFont(m_boldFont);
	
	ATLASSERT(m_durationLabel.IsWindow());
	ATLASSERT(m_duration.IsWindow());
	ATLASSERT(m_durationFiller.IsWindow());
	ATLASSERT(m_costLabel.IsWindow());
	ATLASSERT(m_cost.IsWindow());
	ATLASSERT(m_costFiller.IsWindow());
	
	// Building layout ...
	
	AddLine(5, CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	AddComponent(&m_durationLabel, m_durationLabel.GetWndClassName(), 45);
	//AddComponent(&m_durationFiller, m_durationFiller.GetWndClassName(), 5, 0, 0, TRUE);
	AddComponent(&m_duration, m_duration.GetWndClassName(), 47);
	
	//AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	//AddComponent(&m_costLabel, m_costLabel.GetWndClassName(), 33);
	AddComponent(&m_cost, m_cost.GetWndClassName(), 80);
	//AddComponent(&m_costFiller, m_costFiller.GetWndClassName(), 5, 0, 0, TRUE);
	
	return 0L;
}


LRESULT CUOLFoneStatusPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_durationLabel.DestroyWindow();
	m_durationFiller.DestroyWindow();
	m_duration.DestroyWindow();
	m_costLabel.DestroyWindow();
	m_cost.DestroyWindow();
	
	m_normalFont.DeleteObject();
	m_boldFont.DeleteObject();
	
	return 0L;
}


void CUOLFoneStatusPanel::UpdateCallDuration(int callDuration, double costPerMinute, BOOL bSessionUsingCredits, int nPlanSeconds)
{
	CString str;
	
	int minutesNumber = callDuration % 3600;
	
	str.Format("%02d:%02d:%02d", callDuration / 3600, 
			minutesNumber / ONE_MINUTE, minutesNumber % ONE_MINUTE);
	
	m_duration.SetWindowText(str);

	if (!bSessionUsingCredits)
	{	
		costPerMinute = 0;
	}
	
	str.Format("(%s %.2f%s)", "R$", costPerMinute, "/min");
	
	m_cost.SetWindowText(str);
}

/////////////////////////////////////////////////////////////////////
// CUOLFonePromoAdPanel
/////////////////////////////////////////////////////////////////////

CUOLFonePromoAdPanel::CUOLFonePromoAdPanel(void)
{
}


CUOLFonePromoAdPanel::~CUOLFonePromoAdPanel(void)
{
}

LRESULT CUOLFonePromoAdPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls ...
	//

	Initialize(0, 0, 0, 0);

	m_promoAdWnd.Create(m_hWnd, CRect(0, 0, 163, 45), NULL, WS_CHILDWINDOW | /*WS_VISIBLE |*/ BS_OWNERDRAW);
	m_promoAdWnd.SetWindowHolder(this);
	
	ATLASSERT(m_promoAdWnd.IsWindow());
	
	// Building layout ...
	
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);

	AddComponent(&m_promoAdWnd, m_promoAdWnd.GetWndClassName(), 163, 0, 45);

	return 0L;
}


LRESULT CUOLFonePromoAdPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_promoAdWnd.DestroyWindow();
	
	return 0L;
}

BOOL CUOLFonePromoAdPanel::SetUrl(const CString& strUrl)
{	
	m_promoAdWnd.ShowWindow(SW_SHOW);

	return m_promoAdWnd.SetUrl(strUrl);
}

BOOL CUOLFonePromoAdPanel::ShouldCancelNavigation(CString strURL)
{
	if ((0 == strURL.Find(_T("res://"))) || 
			(0 == strURL.Compare(_T("about:blank"))) || 
			(0 != strURL.IsEmpty()) ||
			(0 == strURL.Compare(_T("http:///"))))
	{
		//CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);

		return TRUE;
	}

	CString strAux = CFacadeWrapper::GetInstance()->GetpromoURL();
	//strAux.LoadString(IDS_URL_PROMO_BANNER);
	if (0 == strURL.Compare(strAux))
	{
		return FALSE;
	}
	else
	{
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);
		return TRUE;
	}
}


/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneWindow::CUOLFoneWindow(const CString& strTabName, const CString& strPluginInternalName):
		m_strTabName(strTabName),
		m_strPluginInternalName(strPluginInternalName),
		m_destroyingWindow(FALSE),
		m_callInPlaceTimer(NULL),
		m_timerHits(0),
		m_userBalance(0),
		m_costPerMinute(0.0), 
		m_childContainerViewPos(NULL), 
		m_hasHiddenAll(FALSE), 
		m_pluginToolBarPos(NULL), 
		m_panelAccountInfoPos(NULL), 
		m_nTabIndex(-1), 
		m_isCallingSac(FALSE), 
		m_bHasPendingUpdate(FALSE),
		m_bIsSessionUsingCredits(TRUE)
#ifdef  _TAB_DEMO_MODE
		,m_bForceBlink(FALSE)
#endif
{
	m_pTooltipProvider = new CUOLFoneTooltipProvider(m_strTabName);
	
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT, 
			CWTLWndComponentsOrganizer::WINDOW_ALIGNMENT_TOP, TRUE);
	
	CFacadeWrapper::GetInstance()->SetUOLFoneWindowPtr(this);
	
#ifndef  _TAB_DEMO_MODE
	m_showingWindowID = -1;
#endif
}


CUOLFoneWindow::~CUOLFoneWindow()
{
}


CString CUOLFoneWindow::GetTabName() const
{
	return m_strTabName;
}


CString CUOLFoneWindow::GetPluginInternalName() const
{
	return m_strPluginInternalName;
}


void CUOLFoneWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
	
	size.cx = __max(size.cx, m_statusPanel.GetMinimumWindowWidth());
}


HWND CUOLFoneWindow::CreateTab(HWND hWndParent, const CRect& rectTab, DWORD dwWindowStyle)
{
	CRect rectWindow = rectTab;
	
	HWND hWnd = Create(hWndParent, rectWindow, NULL, dwWindowStyle);
	
	// This call is made on the plugin class and here, because on the startup 
	// the toolbar is not loaded yet.
	// After the main interface is loaded, we must be sure that the plugin will 
	// control the button's showing status.
	m_nTabIndex = CFacadeWrapper::GetInstance()->AddTabButton();
	
	return hWnd;
}


void CUOLFoneWindow::DestroyTab(HWND hWndTab)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	m_destroyingWindow = TRUE;
	
	if (hWndTab == m_hWnd)
	{
		DestroyWindow();
	}
	CFacadeWrapper::GetInstance()->DestroyRateWindow();
}


HWND CUOLFoneWindow::GetTabHWND()
{
	return m_hWnd;
}


void CUOLFoneWindow::SetTabButtonIndex(int nTabIndex)
{
	m_nTabIndex = nTabIndex;
}


int CUOLFoneWindow::GetTabButtonIndex()
{
	return m_nTabIndex;
}


IUOLMessengerTooltipProviderPtr CUOLFoneWindow::GetTooltipProvider() const
{
	//return m_pTooltipProvider;
	return NULL;
}


void CUOLFoneWindow::UpdateWindowInterface(int showingWindowID)
{
	CUOLFoneCallErrorWindow* pCallErrorWindow = NULL;

	// This is not the ideal solution for this. "::IsWindow" should not be 
	// called from a different thread that is what happens here a causes 
	// the crash.
	// We should change this by a ::PostMessage to a method that will 
	// actually make the UI update.
	//
	if (!::IsWindow(m_hWnd))
	{
		return;
	}
	
	switch (showingWindowID)
	{
		case CUOLFoneChildWindowContainerView::UOLFONE_KEYBOARD_WINDOW :
			
			m_componentsOrganizer.ShowAllComponents(TRUE);
			m_componentsOrganizer.OrganizeComponents();
			
			m_proceedCall.ShowWindow(SW_SHOW);
			m_hangCall.ShowWindow(SW_SHOW);
			
			m_proceedCall.EnableWindow(TRUE);
			m_hangCall.EnableWindow(FALSE);
			
			m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_CALL);
			m_multiStateHangCall.SetMonoStateBitmap(IDB_BTN_HANGUP_INACTIVE);
			
			m_bottomSeparator.ShowWindow(SW_HIDE);
			m_statusPanel.ShowWindow(SW_HIDE);
			
			m_accountInfoPanel.ShowBuyVoipButton(TRUE);
			m_accountInfoPanel.UpdateLayout();

			
			m_promoPanel.SetUrl(CFacadeWrapper::GetInstance()->GetpromoURL());
			m_promoPanel.ShowWindow(SW_SHOW);

			m_isCallingSac = FALSE;
			
			StopCallControl();
			
		break;
		
		case CUOLFoneChildWindowContainerView::UOLFONE_CALL_IN_PROGRESS_WINDOW :
			
			m_componentsOrganizer.ShowAllComponents(TRUE);
			m_componentsOrganizer.OrganizeComponents();
				
			m_proceedCall.ShowWindow(SW_SHOW);
			m_hangCall.ShowWindow(SW_SHOW);
			
			m_proceedCall.EnableWindow(FALSE);
			m_hangCall.EnableWindow(TRUE);
			
			m_multiStateProceedCall.SetMonoStateBitmap(IDB_BTN_CALL_INACTIVE);
			m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
			
			m_bottomSeparator.ShowWindow(SW_HIDE);
			m_statusPanel.ShowWindow(SW_HIDE);
			
			m_accountInfoPanel.ShowBuyVoipButton(FALSE);
			m_accountInfoPanel.UpdateLayout();

			m_promoPanel.ShowWindow(SW_HIDE);

			StopCallControl();
			
		break;
		
		case CUOLFoneChildWindowContainerView::UOLFONE_ACCEPT_CALL_WINDOW :
			
			m_componentsOrganizer.ShowAllComponents(TRUE);
			m_componentsOrganizer.OrganizeComponents();
				
			m_proceedCall.ShowWindow(SW_SHOW);
			m_hangCall.ShowWindow(SW_SHOW);
			
			m_proceedCall.EnableWindow(TRUE);
			m_hangCall.EnableWindow(TRUE);
			
			m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_ACCEPT);
			m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_DONTACCEPT);
			
			m_bottomSeparator.ShowWindow(SW_HIDE);
			m_statusPanel.ShowWindow(SW_HIDE);
			
			m_accountInfoPanel.ShowBuyVoipButton(FALSE);
			m_accountInfoPanel.UpdateLayout();

			m_promoPanel.ShowWindow(SW_HIDE);

			StopCallControl();
			
		break;
		
		case CUOLFoneChildWindowContainerView::UOLFONE_CALL_IN_PLACE_WINDOW :
			
			m_componentsOrganizer.ShowAllComponents(TRUE);
			m_componentsOrganizer.OrganizeComponents();
				
			m_proceedCall.ShowWindow(SW_SHOW);
			m_hangCall.ShowWindow(SW_SHOW);
			
			m_proceedCall.EnableWindow(FALSE);
			m_hangCall.EnableWindow(TRUE);
			
			m_multiStateProceedCall.SetMonoStateBitmap(IDB_BTN_CALL_INACTIVE);
			m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
			
			m_bottomSeparator.ShowWindow(SW_SHOW);
			m_statusPanel.ShowWindow(SW_SHOW);

			m_accountInfoPanel.ShowBuyVoipButton(FALSE);
			m_accountInfoPanel.UpdateLayout();

			m_promoPanel.ShowWindow(SW_HIDE);

			StartCallControl();

		break;
		
		case CUOLFoneChildWindowContainerView::UOLFONE_CALL_ERROR_WINDOW :
			
			pCallErrorWindow = static_cast<CUOLFoneCallErrorWindow*>(
					m_childContainerView.GetChildWindowPtr(
						CUOLFoneChildWindowContainerView::UOLFONE_CALL_ERROR_WINDOW));
			
			m_componentsOrganizer.ShowAllComponents(TRUE);
			m_componentsOrganizer.OrganizeComponents();
			
			if ( pCallErrorWindow != NULL)
			{
				if ( (pCallErrorWindow->GetWindowInfoType() == 
							CUOLFoneCallErrorWindow::INFO_TYPE_PURE_WARNING) || 
						(pCallErrorWindow->GetWindowInfoType() == 
							CUOLFoneCallErrorWindow::INFO_TYPE_PURE_ERROR) )
				{
					m_proceedCall.ShowWindow(SW_HIDE);
					m_hangCall.ShowWindow(SW_HIDE);
					
					m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_CALL_INACTIVE);
					m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP_INACTIVE);
				}
				else
				{
					m_proceedCall.ShowWindow(SW_SHOW);
					m_hangCall.ShowWindow(SW_SHOW);
					
					m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_RECALL);
					m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
				}
			}
			else
			{
				// This should not happen.
				//
				m_proceedCall.ShowWindow(SW_SHOW);
				m_hangCall.ShowWindow(SW_SHOW);
				
				m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_RECALL);
				m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
			}
			
			m_proceedCall.EnableWindow(TRUE);
			m_hangCall.EnableWindow(TRUE);
			
			m_bottomSeparator.ShowWindow(SW_HIDE);
			m_statusPanel.ShowWindow(SW_HIDE);
			
			m_accountInfoPanel.ShowBuyVoipButton(FALSE);
			m_accountInfoPanel.UpdateLayout();
			
			m_promoPanel.ShowWindow(SW_HIDE);
			m_isCallingSac = FALSE;
			
			StopCallControl();
			
		break;
		
		case CUOLFoneChildWindowContainerView::UOLFONE_INITIALIZING_WINDOW :
			
			m_componentsOrganizer.ShowAllComponents(TRUE);
			m_componentsOrganizer.ShowLineComponents(FALSE, m_panelAccountInfoPos);
			m_componentsOrganizer.OrganizeComponents();
				
			m_proceedCall.ShowWindow(SW_HIDE);
			m_hangCall.ShowWindow(SW_HIDE);
			
			m_proceedCall.EnableWindow(TRUE);
			m_hangCall.EnableWindow(TRUE);
			
			m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_RECALL);
			m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
			
			m_bottomSeparator.ShowWindow(SW_HIDE);
			m_statusPanel.ShowWindow(SW_HIDE);
			
			m_accountInfoPanel.ShowBuyVoipButton(TRUE);
			m_accountInfoPanel.UpdateLayout();

			m_promoPanel.ShowWindow(SW_HIDE);
			m_isCallingSac = FALSE;

			StopCallControl();
			
		break;
		
		case CUOLFoneChildWindowContainerView::UOLFONE_INFORMATION_WINDOW :
			
			m_componentsOrganizer.ShowAllComponents(FALSE);
			m_componentsOrganizer.ShowLineComponents(TRUE, m_pluginToolBarPos);
			m_componentsOrganizer.ShowLineComponents(TRUE, m_childContainerViewPos);
			m_componentsOrganizer.OrganizeComponents();
				
			m_proceedCall.ShowWindow(SW_HIDE);
			m_hangCall.ShowWindow(SW_HIDE);
			
			m_proceedCall.EnableWindow(TRUE);
			m_hangCall.EnableWindow(TRUE);
			
			m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_RECALL);
			m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
			
			m_bottomSeparator.ShowWindow(SW_HIDE);
			m_statusPanel.ShowWindow(SW_HIDE);

			m_accountInfoPanel.ShowBuyVoipButton(FALSE);
			m_accountInfoPanel.UpdateLayout();
			m_isCallingSac = FALSE;
			
			StopCallControl();
			
		break;
	}

	m_showingWindowID = showingWindowID;
}


void CUOLFoneWindow::UpdateAccountStats(CUOLFoneAccountPtr pAccount)
{
	m_accountInfoPanel.SetHasVoipOutPlan(pAccount->HasVoipOutPlan());
	
	// This is not the ideal solution for this. "::IsWindow" should not be 
	// called from a different thread that is what happens here a causes 
	// the crash.
	// We should change this by a ::PostMessage to a method that will 
	// actually make the UI update.
	//
	if (!::IsWindow(m_hWnd))
	{
		return;
	}
	
	if (NULL == m_callInPlaceTimer)
	{
		m_strPlanName = pAccount->GetPlanName();
		m_nPlanRemainingSeconds = pAccount->GetPlanRemainingSeconds();
		m_nPlanSeconds = pAccount->GetPlanSeconds();
		m_userBalance = pAccount->GetBalance();
		m_accountInfoPanel.UpdateLabels(m_userBalance, m_nPlanRemainingSeconds, m_strPlanName);
	}
	else
	{
		m_bHasPendingUpdate = TRUE;
	}
}


void CUOLFoneWindow::UpdatePhoneCallStats(CPhoneCallPtr pPhoneCall)
{
	m_bIsSessionUsingCredits = pPhoneCall->IsSessionUsingCredits();
	m_costPerMinute = pPhoneCall->GetCostPerMinute();
	m_accountInfoPanel.UpdateLabels(m_userBalance, m_nPlanRemainingSeconds, m_strPlanName);
}


#ifdef  _TAB_DEMO_MODE
void CUOLFoneWindow::CallNextWindow()
{
	OnButtonClicked(0, ID_BUTTON_MY_ACCOUNT, NULL);
}
#endif


LRESULT CUOLFoneWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString labelStr;
	
	//
	// Create child windows container.
	//
	
	m_childContainerView.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN);
	
	//
	// Creating controls ...
	//
	
	m_toolbarPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	m_accountInfoPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	labelStr.LoadString(IDS_BTN_LABEL_PROCEED_CALL);
	m_proceedCall.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_PROCEED_CALL);
	m_multiStateProceedCall.SubclassWindow(m_proceedCall.m_hWnd);
	m_multiStateProceedCall.SetMultiStateBitmap(IDB_BTN_CALL);
	
	labelStr.LoadString(IDS_BTN_LABEL_HANG_CALL);
	m_hangCall.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_HANG_CALL);
	m_multiStateHangCall.SubclassWindow(m_hangCall.m_hWnd);
	m_multiStateHangCall.SetMultiStateBitmap(IDB_BTN_HANGUP);
	
	m_bottomFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_bottomSeparator.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	m_bottomSeparator.SetBackgroundColor(RGB(0x55, 0xA6, 0xCC));
	
	m_statusPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN);

	m_promoPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN);

    ATLASSERT(m_childContainerView.IsWindow());
	ATLASSERT(m_toolbarPanel.IsWindow());
	ATLASSERT(m_accountInfoPanel.IsWindow());
	ATLASSERT(m_proceedCall.IsWindow());
	ATLASSERT(m_hangCall.IsWindow());
	ATLASSERT(m_bottomFiller.IsWindow());
	ATLASSERT(m_bottomSeparator.IsWindow());
	ATLASSERT(m_statusPanel.IsWindow());
	ATLASSERT(m_promoPanel.IsWindow());
	
	// Building layout ...
	
	// Build line 1.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_toolbarPanel, 
			m_toolbarPanel.GetWndClassName(), 
			m_toolbarPanel.GetMinimumWindowWidth(), 0, 
			m_toolbarPanel.GetMinimumWindowHeight(), TRUE);
	
	// Get the line of the plugin toolbar for future use.
	m_pluginToolBarPos = m_componentsOrganizer.GetLastAddedLinePosition();
	
	// Build line 2.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT);
	m_componentsOrganizer.AddComponent(&m_accountInfoPanel, m_accountInfoPanel.GetWndClassName(), 
			m_accountInfoPanel.GetMinimumWindowWidth(), 0, 
			LINE_FULL_HEIGHT, TRUE);

	// Get the line of the account info panel for future use.
	m_panelAccountInfoPos = m_componentsOrganizer.GetLastAddedLinePosition();
	
	//m_componentsOrganizer.AddSeparatorLine(8);
	
	// Build line 4: add the child windows container view.
	CSize containerSize;
	m_childContainerView.GetMinSize(containerSize);
	
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_BOTTOM, TRUE, 1);
	m_componentsOrganizer.AddComponent(&m_childContainerView, 
			m_childContainerView.GetWndClassName(), containerSize.cx, 0, 
			containerSize.cy, TRUE, &m_childContainerView);
	
	// Get the line of the child container for future use.
	m_childContainerViewPos = m_componentsOrganizer.GetLastAddedLinePosition();
	
	m_componentsOrganizer.AddSeparatorLine(10);
	
	// Build line 6.
	m_componentsOrganizer.AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_proceedCall, 
			m_proceedCall.GetWndClassName(), 
			m_multiStateProceedCall.GetWidth(), 0, 
			m_multiStateProceedCall.GetHeight());
	m_componentsOrganizer.AddComponent(&m_hangCall, 
			m_hangCall.GetWndClassName(), 
			m_multiStateHangCall.GetWidth(), 0, 
			m_multiStateHangCall.GetHeight());
	
	

	// Add a bottom filler line.//Build line Promo Ad
	//m_componentsOrganizer.AddLine(0, 
	//		CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT/*CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED*/, 
	//		CWTLWndComponentsOrganizer::LINE_VORIENTATION_BOTTOM, TRUE, 200);
	m_componentsOrganizer.AddSeparatorLine(15);
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT/*CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED*/, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_BOTTOM, TRUE, 200);
	m_componentsOrganizer.AddComponent(&m_promoPanel, 
			/*m_promoPanel.GetWndClassName()*/"CUOLFonePromoAdPanel", 163, 45, 0, TRUE);
	
	m_promoPanelPos = m_componentsOrganizer.GetLastAddedLinePosition();

	// Build line 8.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_bottomSeparator, 
			m_bottomSeparator.GetWndClassName(), 20, 3, 0, TRUE);
	
	// Build line 9.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_statusPanel, 
			m_statusPanel.GetWndClassName(), 
			m_statusPanel.GetMinimumWindowWidth(), 0, 
			m_statusPanel.GetMinimumWindowHeight(), TRUE);

	m_accountInfoPanel.SetSessionUsingCredits(m_bIsSessionUsingCredits);
	
	return 0;
}


LRESULT CUOLFoneWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// Destroy timers
	StopCallControl();
	
	m_childContainerView.DestroyWindow();
	m_toolbarPanel.DestroyWindow();
	m_accountInfoPanel.DestroyWindow();
	m_proceedCall.DestroyWindow();
	m_hangCall.DestroyWindow();
	m_bottomFiller.DestroyWindow();
	m_bottomSeparator.DestroyWindow();
	m_statusPanel.DestroyWindow();
	m_promoPanel.DestroyWindow();
	
	return 0;
}


LRESULT CUOLFoneWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents(TRUE);
	
	return 0L;
}


LRESULT CUOLFoneWindow::OnShowWindow(UINT /*uMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	if (!m_destroyingWindow) {
		
		BOOL bShow=(BOOL)wShow;
		
		if (!bShow) {
			
			m_childContainerView.ShowWindow(SW_HIDE);
		}
		else {
			
			m_childContainerView.ShowWindow(SW_SHOW);
		}
	}
	
	return 0L;
}


LRESULT CUOLFoneWindow::OnTimer(UINT /*uMsg*/, WPARAM wTimerID, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);

	m_timerHits++;

	if (!m_destroyingWindow)
	{
		// ------------------------------
		// Session is using credits
		if (m_bIsSessionUsingCredits) 
		{
			if ( 0 == ((m_timerHits - 1) % ONE_MINUTE) )
			{
				if ((m_costPerMinute > 0) && (m_userBalance >= m_costPerMinute))
				{
					m_userBalance -= m_costPerMinute;
				}
			}

			BOOL bHangUp = FALSE;
			if ( m_userBalance < m_costPerMinute ) //last minute
			{
				if ( (m_timerHits % ONE_MINUTE) >= 40)
				{
					bHangUp = TRUE;
					CFacadeWrapper::GetInstance()->HangUpPhoneCall();
				}
			}

#ifdef  _TAB_DEMO_MODE
			if (m_bForceBlink)
#else
			if ( m_userBalance < (m_costPerMinute * 3) )
#endif
			{
				if ( !m_accountInfoPanel.IsBlinking() && !bHangUp )
				{
					m_accountInfoPanel.StartBlink();
				}
			}
			else
			{
				if ( m_accountInfoPanel.IsBlinking() )
				{
					m_accountInfoPanel.StopBlink();
				}
			}
		}
		else
		{
			// ------------------------------
			// Session is using plan seconds

			if ((m_timerHits > 1) && (0 == ((m_timerHits - 1) % ONE_MINUTE)))
			{
				if ((m_nPlanRemainingSeconds > 0) && (m_nPlanRemainingSeconds >= ONE_MINUTE))
				{
					m_nPlanRemainingSeconds -= ONE_MINUTE;
				}
			}

#ifdef  _TAB_DEMO_MODE
			if (m_bForceBlink)
#else
			if (m_nPlanRemainingSeconds < (ONE_MINUTE * 3))
#endif
			{
				if ( !m_accountInfoPanel.IsBlinking() )
				{
					m_accountInfoPanel.StartBlink();
				}
			}
			else
			{
				if ( m_accountInfoPanel.IsBlinking() )
				{
					m_accountInfoPanel.StopBlink();
				}
			}
		}
	
	}
	
	m_accountInfoPanel.SetSessionUsingCredits(m_bIsSessionUsingCredits);
	m_accountInfoPanel.UpdateLabels(m_userBalance, m_nPlanRemainingSeconds, m_strPlanName);
	m_statusPanel.UpdateCallDuration(m_timerHits, m_costPerMinute, m_bIsSessionUsingCredits, m_nPlanSeconds);
	
	return 0L;
}


void CUOLFoneWindow::OnButtonClicked(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	CFacadeWrapper::GetInstance()->StopCallSound();
	
	if (nID == ID_BUTTON_PRICES)
	{
#ifdef  _TAB_DEMO_MODE
		// Force accountInfoPanelLine blink
		m_bForceBlink = TRUE;
		m_bIsSessionUsingCredits = !m_bIsSessionUsingCredits;
#else
		CFacadeWrapper::GetInstance()->ShowVoipRates();
#endif
	}
	if (nID == ID_BUTTON_MY_ACCOUNT)
	{

		CString urlExtrato;
		//urlExtrato.LoadString(IDS_URL_SITE_EXTRATO);
		urlExtrato.LoadString(IDS_URL_SITE_MINHA_CONTA);

// TODO: remove test code
#ifdef  _TAB_DEMO_MODE
		static int i = 0;

		switch (i % 13)
		{
		case 0:
			CFacadeWrapper::GetInstance()->ShowKeyboardWindow();
			break;
		case 1:
			CFacadeWrapper::GetInstance()->ShowCallInProgressWindow();
			break;
		case 2:
			CFacadeWrapper::GetInstance()->PlayCallIncomingSound();
			CFacadeWrapper::GetInstance()->ShowAcceptCallWindow();
			break;
		case 3:
			m_userBalance = 2;
			m_nPlanRemainingSeconds = 120;
			m_strPlanName = "Plan Test";
			m_bIsSessionUsingCredits = FALSE;
			CUOLFoneController::GetInstance()->GetCurrentAccount()->SetBalance(2);
			CUOLFoneController::GetInstance()->GetCurrentAccount()->SetPlanSeconds(120);
			CUOLFoneController::GetInstance()->GetCurrentAccount()->SetPlanUsedSeconds(0);
			CFacadeWrapper::GetInstance()->ShowCallInPlaceWindow();
			m_accountInfoPanel.SetHasVoipOutPlan(TRUE);
			m_accountInfoPanel.SetSessionUsingCredits(FALSE);
			m_accountInfoPanel.SetCallInPlace(TRUE);
			m_accountInfoPanel.UpdateLabels(m_userBalance, m_nPlanRemainingSeconds, m_strPlanName);
			break;
		case 4:
			// PUF0101 was categorized as information message.
			//
			CFacadeWrapper::GetInstance()->CategorizeAndShow("UOL Voip", 
					"Este eh um novo texto para testes de atualização da tela de informação! (PUF0101)", NULL);
			break;
		case 5:
			CFacadeWrapper::GetInstance()->ShowInformationWindow("http://uolk.uol.com.br/");
			break;
		case 6:
			// PUF0101 was categorized as information message.
			//
			CFacadeWrapper::GetInstance()->CategorizeAndShow("UOL Voip", 
					"Este eh um novo texto para testes de atualização da tela de informação! (PUF0101)", NULL);
			break;
		case 7:
			CFacadeWrapper::GetInstance()->ShowInformationWindow("http://uolk.uol.com.br/");
			break;
		case 8:
			// PUF0103 was categorized as error message.
			//
			CFacadeWrapper::GetInstance()->CategorizeAndShow("Erro Padrão", 
					"Nova mensagem de erro padrão a ser apresentada quando não houver nada melhor! (PUF0103)", NULL);
			break;
		case 9:
			CFacadeWrapper::GetInstance()->ShowCallErrorWindow("http://uolk.uol.com.br/");
			break;
		case 10:
			// PUF0103 was categorized as error message.
			//
			CFacadeWrapper::GetInstance()->CategorizeAndShow("Erro Padrão", 
					"Nova mensagem de erro padrão a ser apresentada quando não houver nada melhor! (PUF0103)", NULL);
			break;
		case 11:
			CFacadeWrapper::GetInstance()->ShowCallErrorWindow("http://uolk.uol.com.br/");
			break;
		case 12:
			CFacadeWrapper::GetInstance()->ShowInitializingWindow();
			break;
		
		default:
			CFacadeWrapper::GetInstance()->OpenBrowserWithURL(urlExtrato);
		};

		i++;
#else
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(urlExtrato);
#endif
		//CFacadeWrapper::GetInstance()->ShowCallErrorWindow();
		//CFacadeWrapper::GetInstance()->ShowAcceptCallWindow();
		//CFacadeWrapper::GetInstance()->ShowInitializingWindow();
		//CFacadeWrapper::GetInstance()->ShowCallInPlaceWindow();
	}
	else if (nID == ID_BUTTON_PROCEED_CALL)
	{
		if ( CUOLFoneChildWindowContainerView::UOLFONE_ACCEPT_CALL_WINDOW == m_showingWindowID )
		{
			CFacadeWrapper::GetInstance()->AcceptCall();
		}
		else if ( CUOLFoneChildWindowContainerView::UOLFONE_KEYBOARD_WINDOW == m_showingWindowID )
		{
			CFacadeWrapper::GetInstance()->ChooseStartCallType();
		}
		else
		{
			CFacadeWrapper::GetInstance()->Redial();
		}
	}
	else if (nID == ID_BUTTON_PREFERENCES)
	{
		CFacadeWrapper::GetInstance()->ShowPluginConfigDialog();
	}
	else if (nID == ID_BUTTON_HANG_CALL)
	{
		CFacadeWrapper::GetInstance()->HangUpPhoneCall();		
		CFacadeWrapper::GetInstance()->OnStopCall();
	}
	else if (nID == ID_BUTTON_CALL_HISTORY)
	{
		if (m_toolbarPanel.IsAlertingLostCall())
		{
			CString strStatusFilter;
			strStatusFilter.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);
			CFacadeWrapper::GetInstance()->ShowHistoryWindow(strStatusFilter);
		}
		else
		{
			CString strStatusFilter;
			strStatusFilter.LoadString(IDS_HISTORY_CALL_STATUS_ALL);
			CFacadeWrapper::GetInstance()->ShowHistoryWindow(strStatusFilter);
		}
		m_toolbarPanel.OnHistoryAccessed();
	}
	else if (nID == ID_BUTTON_CALL_SAC)
	{
		m_isCallingSac = TRUE;
		CFacadeWrapper::GetInstance()->CallSAC();
	}
	else if (nID == ID_BUTTON_BUY_CREDIT)
	{
		CString strURL;
		strURL.LoadString(IDS_URL_SITE_BUY_CREDIT);
		
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);
	}
	else if (nID == ID_BUTTON_BUY_CREDIT_LINE)
	{
		CString strURL = CFacadeWrapper::GetInstance()->GetBuyCreditsURL();
		
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);
	}
	else if (nID == ID_BUTTON_BUY_PLAN_LINE)
	{
		CString strURL = CFacadeWrapper::GetInstance()->GetBuyPlanURL();
		
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);
	}
}


LRESULT CUOLFoneWindow::OnCtlColorStatic(HDC hDc, HWND)
{
	::SetBkMode(hDc, TRANSPARENT);
	
	return (LRESULT) (HBRUSH) GetStockObject(HOLLOW_BRUSH);
}


void CUOLFoneWindow::StartCallControl()
{
	if (m_callInPlaceTimer != NULL)
	{
		// This should never happen.
		if ( KillTimer(m_callInPlaceTimer) )
		{
			m_callInPlaceTimer = NULL;
		}
	}
	
	m_timerHits = 0;

	// Start call in place timer
	m_callInPlaceTimer = (UINT_PTR) SetTimer(CALL_IN_PLACE_TIMER_ID, CALL_IN_PLACE_UPDATE_TIMEOUT);
	ATLASSERT(m_callInPlaceTimer != NULL);
	
	m_accountInfoPanel.SetCallInPlace(TRUE);
	m_accountInfoPanel.UpdateLabels(m_userBalance, m_nPlanRemainingSeconds, m_strPlanName);
}


void CUOLFoneWindow::StopCallControl()
{
	if (m_callInPlaceTimer != NULL)
	{
		// This should always happen.
		if ( KillTimer(m_callInPlaceTimer) )
		{
			m_callInPlaceTimer = NULL;
		}
		
		if (m_bHasPendingUpdate)
		{
			m_bHasPendingUpdate = FALSE;
			
			if (CUOLFoneController::GetInstance()->GetCurrentAccount())
			{
				UpdateAccountStats(CUOLFoneController::GetInstance()->GetCurrentAccount());
			}
		}
	}
	
	m_accountInfoPanel.SetCallInPlace(FALSE);
	m_accountInfoPanel.StopBlink();
	m_accountInfoPanel.SetSessionUsingCredits(FALSE);
	m_accountInfoPanel.Invalidate(TRUE);
}

void CUOLFoneWindow::OnLostCall()
{
	m_accountInfoPanel.SetCallInPlace(FALSE);
	m_accountInfoPanel.StopBlink();
	m_accountInfoPanel.SetSessionUsingCredits(FALSE);
	m_accountInfoPanel.Invalidate(TRUE);
	m_toolbarPanel.OnLostCall();
}


void CUOLFoneWindow::ShowSACButton(BOOL bShow)
{
	m_toolbarPanel.ShowSACButton(bShow);
}


void CUOLFoneWindow::UpdateBalancePanel(CUOLFoneAccountPtr pAccount)
{
	m_userBalance = pAccount->GetBalance();
	m_strPlanName = pAccount->GetPlanName();
	m_nPlanRemainingSeconds = pAccount->GetPlanRemainingSeconds();

	m_accountInfoPanel.SetHasVoipOutPlan(pAccount->HasVoipOutPlan());
	m_accountInfoPanel.UpdateLabels(m_userBalance, m_nPlanRemainingSeconds, m_strPlanName);
	m_accountInfoPanel.UpdateLayout();
}
