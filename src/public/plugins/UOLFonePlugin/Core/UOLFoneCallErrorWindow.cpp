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
#include ".\UOLFoneCallErrorWindow.h"

#include "../FacadeWrapper.h"
#include "../Resource.h"
#include "../UOLFonePluginTheme.h"


/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow positioning defines
/////////////////////////////////////////////////////////////////////


#define		DEFAULT_SHIFT						0

#define		LEFT_SHIFT							DEFAULT_SHIFT
#define		TOP_SHIFT							20
#define		RIGHT_SHIFT							DEFAULT_SHIFT
#define		BOTTOM_SHIFT						DEFAULT_SHIFT

#define		COMPONENT_DEFAULT_HEIGHT			25
#define		HIGHER_STATIC_SIZE					20
#define		COMPONENTS_HORIZONTAL_SEPARATION	10
#define		COMPONENTS_VERTICAL_SEPARATION		10

#define		CONTACT_IMAGE_WIDTH					92
#define		CONTACT_IMAGE_HEIGHT				92




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallErrorStatusPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneCallErrorStatusPanel::CUOLFoneCallErrorStatusPanel(void)
{
}


CUOLFoneCallErrorStatusPanel::~CUOLFoneCallErrorStatusPanel(void)
{
}


void CUOLFoneCallErrorStatusPanel::SetStatusString(LPCTSTR statusStr)
{
	m_strStatus = statusStr;
	
	if (m_callErrorText.IsWindow())
	{
		m_callErrorText.SetWindowText(m_strStatus);
	}
}


void CUOLFoneCallErrorStatusPanel::SetColors(COLORREF textColor, COLORREF backgroundColor)
{
	m_textColor = textColor;
	m_backgroundColor = backgroundColor;
	
	if (IsWindow())
	{
		SetTextColor(m_textColor);
		SetBackgroundColor(m_backgroundColor);
	}
}


void CUOLFoneCallErrorStatusPanel::SetImage(UINT uiImageID)
{
	if (m_callErrorBitmap.m_hBitmap != NULL)
	{
		m_callErrorBitmap.DeleteObject();
	}
	
	m_callErrorBitmap.LoadBitmap(uiImageID);
	
	if (m_callErrorImage.IsWindow())
	{
		m_callErrorImage.SetBitmap(m_callErrorBitmap.m_hBitmap);
	}
}


void CUOLFoneCallErrorStatusPanel::ReorganizeAndUpdate()
{
	RecalculateLinesHeightsAndWidths();
	OrganizeComponents();

	UpdateWindow();
}


LRESULT CUOLFoneCallErrorStatusPanel::OnCreate(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString labelStr;
	
	Initialize(0, 5, 0, 5);
	SetTextColor(m_textColor);
	SetBackgroundColor(m_backgroundColor);
	
	//
	// Creating controls ...
	//
	
	m_textFont.CreatePointFont(100, "Tahoma", 0, true);
	
	m_callErrorImage.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_BITMAP);
	m_callErrorImage.SetBitmap(m_callErrorBitmap.m_hBitmap);
	
	m_callErrorText.Create(m_hWnd, CRect(0, 0, 0, 0), m_strStatus, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_callErrorText.SetFont(m_textFont);
	
	ATLASSERT(m_callErrorImage.IsWindow());
	ATLASSERT(m_callErrorText.IsWindow());
	
	// Building layout ...
	
	// Build line 1 (the only one, in fact).
	AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	AddComponent(&m_callErrorImage, m_callErrorImage.GetWndClassName(), 
			15);
	AddComponent(&m_callErrorText, m_callErrorText.GetWndClassName());
	
	return 0L;
}


LRESULT CUOLFoneCallErrorStatusPanel::OnDestroy(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_callErrorImage.DestroyWindow();
	m_callErrorText.DestroyWindow();
	
	m_textFont.DeleteObject();
	
	m_callErrorBitmap.DeleteObject();
	
	return 0L;
}


LRESULT CUOLFoneCallErrorStatusPanel::OnShowWindow(
		UINT /*uMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	/*
	BOOL bShow=(BOOL)wShow;
	
	if (bShow)
	{
		m_callErrorText.SetWindowText("Teste");
	}
	*/
	
	return 0L;
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallErrorWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneCallErrorWindow::CUOLFoneCallErrorWindow() : 
		m_errorDescription("Essa é a mensagem de erro padrão do sistema!"), 
		m_knowMoreURL("http://www.uol.com.br/")
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);
	
	m_errorBackgroundColor = RGB(0xF9, 0xE4, 0xE4);
	m_errorTextColor = RGB(0xC5, 0x00, 0x00);
	
	m_warningBackgroundColor = RGB(0xFF, 0xFC, 0xED);
	m_warningTextColor = RGB(0x00, 0x00, 0x00);
	
	m_errorStatusString.LoadString(IDS_LABEL_ERRO_ENCONTRADO);
	m_warningStatusString.LoadString(IDS_LABEL_WARNING);
	
	m_uiWindowInfoType = INFO_TYPE_OTHER;
}


CUOLFoneCallErrorWindow::~CUOLFoneCallErrorWindow() 
{
}


void CUOLFoneCallErrorWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneCallErrorWindow::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneCallErrorWindow::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneCallErrorWindow::SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo)
{
	if (pChildWindowInfo == NULL)
	{
		return;
	}
	
	if (pChildWindowInfo->GetHoldWebBrowser())
	{
		m_knowMoreURL = pChildWindowInfo->GetKnowMoreURL();
		
		m_webBrowserWnd.SetUrl(m_knowMoreURL);
		
		m_componentsOrganizer.SetBoundaries(0, 0, 0, 0);
		
		m_componentsOrganizer.ShowAllComponents(FALSE);
		m_componentsOrganizer.ShowLineComponents(TRUE, m_statusPanelLine);
		m_componentsOrganizer.ShowLineComponents(TRUE, m_webBrowserWindowLine);
	}
	else
	{
		BOOL mudou = FALSE;
		
		CString strTitle = pChildWindowInfo->GetTitle();
		
		if (!strTitle.IsEmpty())
		{
			m_statusPanel.SetStatusString(strTitle);
		}
		
		CString strMessage = pChildWindowInfo->GetMessage();
		
		if (!strMessage.IsEmpty())
		{
			m_errorDescriptionLabel.SetText(strMessage);
			mudou = TRUE;
		}
		
		m_componentsOrganizer.SetBoundaries(LEFT_SHIFT, TOP_SHIFT, 
				RIGHT_SHIFT, BOTTOM_SHIFT);
		
		m_componentsOrganizer.ShowAllComponents(TRUE);
		
		ITabChildWindowActionCallback* pActionCallback = pChildWindowInfo->GetActionCallback();
		
		if (pActionCallback)
		{
			m_componentsOrganizer.ShowLineComponents(TRUE, m_saibaMaisButtonLine);
			
			m_pActionCallback = pActionCallback;
			m_nActionId = pChildWindowInfo->GetCallbackActionId();
			
			DWORD dButtonResourceID = pActionCallback->GetButtonImageResourceID(m_nActionId);
			
			if (dButtonResourceID)
			{
				m_saibaMaisMultiStateButton.SetMultiStateBitmap(dButtonResourceID);
				
				m_componentsOrganizer.UpdateComponent(m_saibaMaisButtonLine, 
						m_saibaMaisButtonComponent, 
						m_saibaMaisMultiStateButton.GetWidth(), 
						m_saibaMaisMultiStateButton.GetHeight());
				
				// We don't need to sinalize the change, because UpdateComponent() already 
				// recalculates everything.
				//
				//mudou = TRUE;
			}
		}
		else
		{
			m_componentsOrganizer.ShowLineComponents(FALSE, m_saibaMaisButtonLine);
		}
		
		m_componentsOrganizer.ShowLineComponents(FALSE, m_webBrowserWindowLine);
	}
	
	m_uiWindowInfoType = pChildWindowInfo->GetWindowType();
	
	if ( (m_uiWindowInfoType == INFO_TYPE_CALL_WARNING) || 
			(m_uiWindowInfoType == INFO_TYPE_PURE_WARNING) )
	{
		m_statusPanel.SetStatusString(m_warningStatusString);
		m_statusPanel.SetColors(m_warningTextColor, m_warningBackgroundColor);
		m_statusPanel.SetImage(IDB_ICO_WARNING);
	}
	else 
	{
		m_statusPanel.SetStatusString(m_errorStatusString);
		m_statusPanel.SetColors(m_errorTextColor, m_errorBackgroundColor);
		m_statusPanel.SetImage(IDB_ICO_ERROR);
	}
	
	m_statusPanel.ReorganizeAndUpdate();
	m_statusPanel.Invalidate();
	
	m_componentsOrganizer.OrganizeComponents();
}


BOOL CUOLFoneCallErrorWindow::CanGetFocus()
{
	return TRUE;
}


void CUOLFoneCallErrorWindow::ResetWindow()
{
	m_componentsOrganizer.ShowLineComponents(FALSE, m_saibaMaisButtonLine);
	
	m_componentsOrganizer.OrganizeComponents();
}


UINT CUOLFoneCallErrorWindow::GetWindowInfoType()
{
	return m_uiWindowInfoType;
}


LRESULT CUOLFoneCallErrorWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls...
	//
	
	m_descriptionFont.CreatePointFont(100, "Tahoma");
	
	m_errorDescriptionLabel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_errorDescriptionLabel.SetFont(m_descriptionFont);
	m_errorDescriptionLabel.SetMaxWidth(80);
	m_errorDescriptionLabel.SetText(m_errorDescription);
	
	m_sidesFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_lineSeparator.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_saibaMaisButton.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_SAIBA_MAIS);
	m_saibaMaisMultiStateButton.SubclassWindow(m_saibaMaisButton);
	m_saibaMaisMultiStateButton.SetMultiStateBitmap(IDB_BTN_SAIBA_MAIS);
	
	m_centerFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	// Initialize with error defaults.
	m_statusPanel.SetStatusString(m_errorStatusString);
	m_statusPanel.SetColors(m_errorTextColor, m_errorBackgroundColor);
	m_statusPanel.SetImage(IDB_ICO_ERROR);
	m_statusPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	m_webBrowserWnd.Create(m_hWnd, CRect(0, 0, 0, 0));
	m_webBrowserWnd.SetWindowHolder(this);
	
	ATLASSERT(m_errorDescriptionLabel.IsWindow());
	ATLASSERT(m_sidesFiller.IsWindow());
	ATLASSERT(m_lineSeparator.IsWindow());
	ATLASSERT(m_saibaMaisButton.IsWindow());
	ATLASSERT(m_centerFiller.IsWindow());
	ATLASSERT(m_statusPanel.IsWindow());
	ATLASSERT(m_webBrowserWnd.IsWindow());
	
	//
	// Building layout...
	//
	
	// Build line 1.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 20);
	m_componentsOrganizer.AddComponent(&m_errorDescriptionLabel, 
			m_errorDescriptionLabel.GetWndClassName(), 
			m_errorDescriptionLabel.GetWidth(), 0, 
			m_errorDescriptionLabel.GetHeight(), TRUE);
	m_componentsOrganizer.AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 20);
	
	// Build line 2.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_lineSeparator, 
			m_lineSeparator.GetWndClassName(), 20, 0, 
			COMPONENTS_VERTICAL_SEPARATION, TRUE);
	
	// Build line 3.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_saibaMaisButton, 
			m_saibaMaisButton.GetWndClassName(), 
			m_saibaMaisMultiStateButton.GetWidth(), 0, 
			m_saibaMaisMultiStateButton.GetHeight());
	
	// Save the button's position for later dimensions update.
	m_saibaMaisButtonLine = m_componentsOrganizer.GetLastAddedLinePosition();
	m_saibaMaisButtonComponent = m_componentsOrganizer.GetLastAddedComponentPosition();
	
	// Build line 4.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_BOTTOM, TRUE);
	m_componentsOrganizer.AddComponent(&m_centerFiller, 
			m_centerFiller.GetWndClassName(), 40, 0, 0, TRUE);
	
	// Build line 5.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_TOP, TRUE);
	m_componentsOrganizer.AddComponent(&m_webBrowserWnd, 
			m_webBrowserWnd.GetWndClassName(), 10, 0, 0, TRUE);
	
	// Save the web browser window line for further visibility control.
	m_webBrowserWindowLine = m_componentsOrganizer.GetLastAddedLinePosition();
	
	// Build line 6.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_statusPanel, 
			m_statusPanel.GetWndClassName(), m_statusPanel.GetMinimumWindowWidth(), 
			0, m_statusPanel.GetMinimumWindowHeight(), TRUE);
	
	// Save the status panel line for further visibility control.
	m_statusPanelLine = m_componentsOrganizer.GetLastAddedLinePosition();
	
	// Web browser line is initially hidden.
	m_componentsOrganizer.ShowLineComponents(FALSE, m_webBrowserWindowLine);
	
	return 0;
}


LRESULT CUOLFoneCallErrorWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_errorDescriptionLabel.DestroyWindow();
	m_sidesFiller.DestroyWindow();
	m_lineSeparator.DestroyWindow();
	m_saibaMaisButton.DestroyWindow();
	m_centerFiller.DestroyWindow();
	m_statusPanel.DestroyWindow();
	m_webBrowserWnd.DestroyWindow();
	
	m_descriptionFont.DeleteObject();
	
	return 0;
}


LRESULT CUOLFoneCallErrorWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents(TRUE);
	
	return 0L;
}


LRESULT CUOLFoneCallErrorWindow::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HBRUSH hBrush = NULL;
	
	if (WM_CTLCOLOREDIT == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetEditColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetEditBrush();
	}
	else if (WM_CTLCOLORLISTBOX == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetListBoxColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetListBoxBrush();
	}
	else if (WM_CTLCOLORBTN == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetButtonColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetButtonBrush();
	}
	else if (WM_CTLCOLORDLG == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetWindowColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetWindowBrush();
	}
	else if (WM_CTLCOLORSCROLLBAR == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetScrollBarColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetScrollBarBrush();
	}
	else if (WM_CTLCOLORSTATIC == nMsg)
	{
		::SetBkColor((HDC) wParam, 
				CUOLFonePluginTheme::GetInstance()->GetStaticColor());
		hBrush = CUOLFonePluginTheme::GetInstance()->GetStaticBrush();
	}
	
	return (LRESULT) hBrush;
}


void CUOLFoneCallErrorWindow::OnButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	// This test should not be necessary, because the button will be 
	// visible and "clickable" when this member isn't null.
	// Anyway ...
	if (m_pActionCallback)
	{
		m_pActionCallback->ExecuteInfomationButtonAction(m_nActionId);
	}
	
	/*
	CString uolfoneLabelText;
	
#ifdef  _TAB_DEMO_MODE
	static int i = 0;
	
	switch (i % 6)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			m_errorDescriptionLabel.GetText(uolfoneLabelText);
			
			uolfoneLabelText += " vamos adicionar mais um pouco de texto";
			
			m_errorDescriptionLabel.SetText(uolfoneLabelText);
			
			m_componentsOrganizer.OrganizeComponents();
		break;
		
		default:
			CFacadeWrapper::GetInstance()->OpenBrowserWithURL("http://www.uol.com.br");
	}
	
	i++;
#else
	CFacadeWrapper::GetInstance()->OpenBrowserWithURL("http://www.uol.com.br");
#endif
	*/
}


BOOL CUOLFoneCallErrorWindow::ShouldCancelNavigation(CString strURL)
{
	if ((0 != strURL.Find(_T("res://"))) && 
			(0 != strURL.Compare(_T("about:blank"))) && 
			(0 != strURL.Compare(m_knowMoreURL)) )
	{
		CFacadeWrapper::GetInstance()->OpenBrowserWithURL(strURL);
		
		return TRUE;
	}
	
	return FALSE;
}


void CUOLFoneCallErrorWindow::SetWindowInfoType(UINT uiWindowInfoType)
{
	m_uiWindowInfoType = uiWindowInfoType;
}

