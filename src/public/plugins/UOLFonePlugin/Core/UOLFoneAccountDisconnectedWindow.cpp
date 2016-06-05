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
#include ".\UOLFoneAccountDisconnectedWindow.h"

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
// CUOLFoneAccountDisconnectedStatusPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountDisconnectedStatusPanel::CUOLFoneAccountDisconnectedStatusPanel(void)
{
	m_backgroundColor = RGB(0x55, 0xA6, 0xCC);
	m_textColor = RGB(0xFF, 0xFF, 0xFF);
}


CUOLFoneAccountDisconnectedStatusPanel::~CUOLFoneAccountDisconnectedStatusPanel(void)
{
}


void CUOLFoneAccountDisconnectedStatusPanel::SetStatusString(LPCTSTR statusStr)
{
	m_initializingText.SetWindowText(statusStr);
	OrganizeComponents();
}


LRESULT CUOLFoneAccountDisconnectedStatusPanel::OnCreate(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString labelStr;
	
	SetBackgroundColor(m_backgroundColor);
	SetTextColor(m_textColor);
	Initialize(0, 5, 0, 5);
	
	//
	// Creating controls ...
	//
	
	m_textFont.CreatePointFont(100, "Tahoma", 0, true);
	
	m_initializingImage.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_BITMAP);
	
	m_pictureWnd.SubclassWindow(m_initializingImage.m_hWnd);
	if (m_pictureWnd.Load(MAKEINTRESOURCE(IDR_WAIT_ANIMA),_T("BINARY")))
	{
		m_pictureWnd.Draw();
	}
	
	labelStr.LoadString(IDS_LABEL_ACCOUNT_DISCONNECTED);
	m_initializingText.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_initializingText.SetFont(m_textFont);
	
	ATLASSERT(m_initializingImage.IsWindow());
	ATLASSERT(m_initializingText.IsWindow());
	
	// Building layout ...
	
	SIZE pictureWndSize = m_pictureWnd.GetSize();
	
	// Build line 1 (the only one, in fact).
	AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	AddComponent(&m_initializingImage, m_initializingImage.GetWndClassName(), 
			pictureWndSize.cx, 0, pictureWndSize.cy);
	AddComponent(&m_initializingText, m_initializingText.GetWndClassName());
	
	return 0L;
}


LRESULT CUOLFoneAccountDisconnectedStatusPanel::OnDestroy(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_pictureWnd.UnLoad();
	
	m_initializingImage.DestroyWindow();
	m_initializingText.DestroyWindow();
	
	m_textFont.DeleteObject();
	
	return 0L;
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountDisconnectedWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountDisconnectedWindow::CUOLFoneAccountDisconnectedWindow() : 
		m_destroyingWindow(FALSE)
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);
}


CUOLFoneAccountDisconnectedWindow::~CUOLFoneAccountDisconnectedWindow() 
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	m_destroyingWindow = TRUE;
}


void CUOLFoneAccountDisconnectedWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneAccountDisconnectedWindow::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneAccountDisconnectedWindow::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneAccountDisconnectedWindow::SetChildWindowInfo(CUOLFoneChildWindowInfo childWindowInfo)
{
	CString strTitle = childWindowInfo.GetTitle();
	
	if (!strTitle.IsEmpty())
	{
		m_statusPanel.SetStatusString(strTitle);
	}
}


LRESULT CUOLFoneAccountDisconnectedWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls...
	//
	
	m_centerFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_statusPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	ATLASSERT(m_centerFiller.IsWindow());
	ATLASSERT(m_statusPanel.IsWindow());
	
	//
	// Building layout...
	//
	
	// Build line 1.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, TRUE);
	m_componentsOrganizer.AddComponent(&m_centerFiller, 
			m_centerFiller.GetWndClassName(), 40, 0, COMPONENT_DEFAULT_HEIGHT, TRUE);
	
	// Build line 2.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_statusPanel, 
			m_statusPanel.GetWndClassName(), m_statusPanel.GetMinimumWindowWidth(), 
			0, m_statusPanel.GetMinimumWindowHeight(), TRUE);
	
	return 0;
}


LRESULT CUOLFoneAccountDisconnectedWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_centerFiller.DestroyWindow();
	m_statusPanel.DestroyWindow();
	
	return 0;
}


LRESULT CUOLFoneAccountDisconnectedWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	return 0L;
}


LRESULT CUOLFoneAccountDisconnectedWindow::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

