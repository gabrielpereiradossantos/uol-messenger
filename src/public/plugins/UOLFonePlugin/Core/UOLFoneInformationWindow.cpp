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
#include ".\UOLFoneInformationWindow.h"

#include "../FacadeWrapper.h"
#include "../Resource.h"
#include "../UOLFonePluginTheme.h"


/////////////////////////////////////////////////////////////////////
// CUOLFoneInformationWindow positioning defines
/////////////////////////////////////////////////////////////////////


#define		DEFAULT_SHIFT						20

#define		LEFT_SHIFT							DEFAULT_SHIFT
#define		TOP_SHIFT							10
#define		RIGHT_SHIFT							DEFAULT_SHIFT
#define		BOTTOM_SHIFT						10

#define		COMPONENT_DEFAULT_HEIGHT			25
#define		HIGHER_STATIC_SIZE					20
#define		COMPONENTS_HORIZONTAL_SEPARATION	10
#define		COMPONENTS_VERTICAL_SEPARATION		10

#define		CONTACT_IMAGE_WIDTH					92
#define		CONTACT_IMAGE_HEIGHT				92




/////////////////////////////////////////////////////////////////////
// CUOLFoneInformationWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneInformationWindow::CUOLFoneInformationWindow() : 
		m_infoText("Este eh um texto de teste e deve ser testado com o label dinamico."), 
		m_knowMoreURL("http://www.uol.com.br/")
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);
	
	SetDegradeInitialColor(RGB(0xEC, 0XF3, 0XFB));
}


CUOLFoneInformationWindow::~CUOLFoneInformationWindow() 
{
}


void CUOLFoneInformationWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneInformationWindow::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneInformationWindow::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneInformationWindow::SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo)
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
		m_componentsOrganizer.ShowLineComponents(TRUE, m_webBrowserWindowLine);
	}
	else
	{
		BOOL mudou = FALSE;
		
		CString strTitle = pChildWindowInfo->GetTitle();
		
		if (!strTitle.IsEmpty())
		{
			m_uolfoneLabel.SetWindowText(strTitle);
			mudou = TRUE;
		}
		
		CString strMessage = pChildWindowInfo->GetMessage();
		
		//if (!strMessage.IsEmpty())
		{
			m_infoLabel.SetText(strMessage);
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
	
	m_componentsOrganizer.OrganizeComponents();
}


BOOL CUOLFoneInformationWindow::CanGetFocus()
{
	return TRUE;
}


void CUOLFoneInformationWindow::ResetWindow()
{
	m_componentsOrganizer.ShowLineComponents(FALSE, m_saibaMaisButtonLine);
	
	m_componentsOrganizer.OrganizeComponents();
}


LRESULT CUOLFoneInformationWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString labelStr;
	
	//
	// Creating controls...
	//
	
	m_uolfoneFont.CreatePointFont(150, "Tahoma", 0, true);
	m_infoFont.CreatePointFont(100, "Tahoma");
	
	labelStr.LoadString(IDS_PLUGIN_NAME);
	m_uolfoneLabel.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_uolfoneLabel.SetFont(m_uolfoneFont);
	
	m_separator1.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_infoLabel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_infoLabel.SetFont(m_infoFont);
	m_infoLabel.SetMaxWidth(80);
	m_infoLabel.SetText(m_infoText);
	
	m_separator2.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_saibaMaisButton.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_SAIBA_MAIS);
	m_saibaMaisMultiStateButton.SubclassWindow(m_saibaMaisButton);
	m_saibaMaisMultiStateButton.SetMultiStateBitmap(IDB_BTN_SAIBA_MAIS);
	
	m_webBrowserWnd.Create(m_hWnd, CRect(0, 0, 0, 0));
	m_webBrowserWnd.SetWindowHolder(this);
	
	ATLASSERT(m_uolfoneLabel.IsWindow());
	ATLASSERT(m_separator1.IsWindow());
	ATLASSERT(m_infoLabel.IsWindow());
	ATLASSERT(m_separator2.IsWindow());
	ATLASSERT(m_saibaMaisButton.IsWindow());
	ATLASSERT(m_webBrowserWnd.IsWindow());
	
	//
	// Building layout...
	//
	
	// Build line 1.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_uolfoneLabel, 
			m_uolfoneLabel.GetWndClassName(), 100);
	
	// Add line 2.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_separator1, 
			m_separator1.GetWndClassName(), 20, 0, 
			COMPONENTS_VERTICAL_SEPARATION, TRUE);
	
	// Add line 3.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_infoLabel, 
			m_infoLabel.GetWndClassName(), 
			m_infoLabel.GetWidth(), 0, 
			m_infoLabel.GetHeight(), TRUE);
	
	// Add line 4.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_separator1, 
			m_separator1.GetWndClassName(), 20, 0, 
			COMPONENTS_VERTICAL_SEPARATION, TRUE);
	
	// Build line 5.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_saibaMaisButton, 
			m_saibaMaisButton.GetWndClassName(), 
			m_saibaMaisMultiStateButton.GetWidth(), 0, 
			m_saibaMaisMultiStateButton.GetHeight());
	
	// Save the button's position for later dimensions update.
	m_saibaMaisButtonLine = m_componentsOrganizer.GetLastAddedLinePosition();
	m_saibaMaisButtonComponent = m_componentsOrganizer.GetLastAddedComponentPosition();
	
	// Build line 6.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_TOP, TRUE);
	m_componentsOrganizer.AddComponent(&m_webBrowserWnd, 
			m_webBrowserWnd.GetWndClassName(), 10, 0, 0, TRUE);
	
	// Save the web browser window line for further visibility control.
	m_webBrowserWindowLine = m_componentsOrganizer.GetLastAddedLinePosition();
	
	// Web browser line is initially hidden.
	m_componentsOrganizer.ShowLineComponents(FALSE, m_webBrowserWindowLine);
	
	return 0;
}


LRESULT CUOLFoneInformationWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_uolfoneLabel.DestroyWindow();
	m_separator1.DestroyWindow();
	m_infoLabel.DestroyWindow();
	m_separator2.DestroyWindow();
	m_saibaMaisButton.DestroyWindow();
	m_webBrowserWnd.DestroyWindow();
	
	m_uolfoneFont.DeleteObject();
	m_infoFont.DeleteObject();
	
	return 0;
}


LRESULT CUOLFoneInformationWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	m_uolfoneLabel.Invalidate();
	
	return 0L;
}


void CUOLFoneInformationWindow::OnButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CString uolfoneLabelText;
	
//#ifdef  _TAB_DEMO_MODE
//	static int i = 0;
//	
//	switch (i % 7)
//	{
//		case 0:
//		case 1:
//		case 2:
//		case 3:
//		case 4:
//		case 5:
//			m_infoLabel.GetText(uolfoneLabelText);
//			
//			uolfoneLabelText += " vamos adicionar mais um pouco de texto para que possamos avaliar o redimensionamento correto do componente de label dinamico";
//			
//			m_infoLabel.SetText(uolfoneLabelText);
//			
//			m_componentsOrganizer.OrganizeComponents();
//		break;
//		
//		default:
//			CUOLFoneWindow::CallNextWindow();
//	}
//	
//	i++;
//#else
	
	// This test should not be necessary, because the button will be 
	// visible and "clickable" when this member isn't null.
	// Anyway ...
	if (m_pActionCallback)
	{
		m_pActionCallback->ExecuteInfomationButtonAction(m_nActionId);
	}
	
//#endif
}


/*
LRESULT CUOLFoneInformationWindow::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
*/


BOOL CUOLFoneInformationWindow::ShouldCancelNavigation(CString strURL)
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

