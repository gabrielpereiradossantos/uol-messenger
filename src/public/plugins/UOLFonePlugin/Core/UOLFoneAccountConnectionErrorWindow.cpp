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
#include ".\UOLFoneAccountConnectionErrorWindow.h"

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
// CUOLFoneAccountConnectionErrorUserPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountConnectionErrorUserPanel::CUOLFoneAccountConnectionErrorUserPanel(void)
{
	m_backgroundColor = RGB(0xFF, 0xFF, 0xFF);
}


CUOLFoneAccountConnectionErrorUserPanel::~CUOLFoneAccountConnectionErrorUserPanel(void)
{
}


LRESULT CUOLFoneAccountConnectionErrorUserPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetBackgroundColor(m_backgroundColor);
	
	//
	// Creating controls ...
	//
	
	m_userNameFont.CreatePointFont(80, "Tahoma", 0, true);
	
	// Somente um filler sobre o qual serah desenhada a imagem do contato.
	m_callUserImage.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_lineSeparator.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_sidesFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_callUserName.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER | SS_WORDELLIPSIS);
	m_callUserName.SetFont(m_userNameFont);
	
	ATLASSERT(m_callUserImage.IsWindow());
	ATLASSERT(m_lineSeparator.IsWindow());
	ATLASSERT(m_sidesFiller.IsWindow());
	ATLASSERT(m_callUserName.IsWindow());
	
	// Building layout ...
	
	AddLine(0, CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	AddComponent(&m_callUserImage, m_callUserImage.GetWndClassName(), 
			CONTACT_IMAGE_WIDTH, 0, CONTACT_IMAGE_HEIGHT);
	
	// Add line 2.
	AddLine();
	AddComponent(&m_lineSeparator, 
			m_lineSeparator.GetWndClassName(), 20, 0, 
			COMPONENTS_VERTICAL_SEPARATION, TRUE);
	
	// Build line 3.
	AddLine();
	AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 
			COMPONENTS_HORIZONTAL_SEPARATION, 0, 
			COMPONENT_DEFAULT_HEIGHT);
	AddComponent(&m_callUserName, m_callUserName.GetWndClassName(), 
			140, 0, COMPONENT_DEFAULT_HEIGHT, TRUE);
	AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 
			COMPONENTS_HORIZONTAL_SEPARATION, 0, 
			COMPONENT_DEFAULT_HEIGHT);
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorUserPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_callUserImage.DestroyWindow();
	m_lineSeparator.DestroyWindow();
	m_sidesFiller.DestroyWindow();
	m_callUserName.DestroyWindow();
	
	m_userNameFont.DeleteObject();
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorUserPanel::OnPaint(HDC hDc)
{
	PAINTSTRUCT paintStruct;
	
	BeginPaint(&paintStruct);
	
	CDCHandle thisCDC(::GetWindowDC(m_hWnd));
	
	if (!thisCDC.IsNull())
	{
		IUOLMessengerImagePtr pContactImage = NULL;

		pContactImage = CFacadeWrapper::GetInstance()->GetVoIPCallImage();
		ATLASSERT( pContactImage );

		IUOLMessengerImageDrawStrategyPtr pDrawStrategy = 
			CFacadeWrapper::GetInstance()->CreateDrawStrategy(
			IUOLMessengerUIManager::IDS_DRAW_OPAQUE);

		if ( (pContactImage != NULL) && (pDrawStrategy != NULL) )
		{
			CRect windowRect;
			GetWindowRect(&windowRect);

			CRect imageRect;
			m_callUserImage.GetWindowRect(&imageRect);

			pContactImage->Draw(thisCDC, 
				imageRect.left - windowRect.left, 
				imageRect.top - windowRect.top, 
				CONTACT_IMAGE_WIDTH, CONTACT_IMAGE_HEIGHT, 
				pDrawStrategy);
		}
	}
	
	EndPaint(&paintStruct);
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorUserPanel::OnEraseBkgnd(HDC hDc)
{
	CRect imageRect;
	GetComponentClientPosition(m_callUserImage, imageRect);
	
	::ExcludeClipRect(hDc, imageRect.left, imageRect.top, imageRect.right, imageRect.bottom);
	
	return __super::OnEraseBackground(hDc);
}


void CUOLFoneAccountConnectionErrorUserPanel::UpdateWindowData()
{
	m_callUserName.SetWindowText( CFacadeWrapper::GetInstance()->GetVoIPCallText() );
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountConnectionErrorStatusPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountConnectionErrorStatusPanel::CUOLFoneAccountConnectionErrorStatusPanel(void)
{
	m_backgroundColor = RGB(0xF9, 0xE4, 0xE4);
	m_textColor = RGB(0xC5, 0x00, 0x00);
}


CUOLFoneAccountConnectionErrorStatusPanel::~CUOLFoneAccountConnectionErrorStatusPanel(void)
{
}


void CUOLFoneAccountConnectionErrorStatusPanel::SetStatusString(LPCTSTR statusStr)
{
	m_accountConnectionErrorText.SetWindowText(statusStr);
	OrganizeComponents();
}


LRESULT CUOLFoneAccountConnectionErrorStatusPanel::OnCreate(
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
	
	m_accountConnectionErrorBitmap.LoadBitmap(IDB_ICO_ERROR);
	
	m_accountConnectionErrorImage.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_BITMAP);
	m_accountConnectionErrorImage.SetBitmap(m_accountConnectionErrorBitmap.m_hBitmap);
	
	labelStr.LoadString(IDS_LABEL_ERRO_ENCONTRADO);
	m_accountConnectionErrorText.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_accountConnectionErrorText.SetFont(m_textFont);
	
	ATLASSERT(m_accountConnectionErrorImage.IsWindow());
	ATLASSERT(m_accountConnectionErrorText.IsWindow());
	
	// Building layout ...
	
	// Build line 1 (the only one, in fact).
	AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	AddComponent(&m_accountConnectionErrorImage, 
			m_accountConnectionErrorImage.GetWndClassName(), 15);
	AddComponent(&m_accountConnectionErrorText, 
			m_accountConnectionErrorText.GetWndClassName());
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorStatusPanel::OnDestroy(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_accountConnectionErrorImage.DestroyWindow();
	m_accountConnectionErrorText.DestroyWindow();
	
	m_textFont.DeleteObject();
	
	m_accountConnectionErrorBitmap.DeleteObject();
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorStatusPanel::OnShowWindow(
		UINT /*uMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	/*
	BOOL bShow=(BOOL)wShow;
	
	if (bShow)
	{
		m_accountConnectionErrorText.SetWindowText("Teste");
	}
	*/
	
	return 0L;
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneAccountConnectionErrorWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneAccountConnectionErrorWindow::CUOLFoneAccountConnectionErrorWindow()
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);
}


CUOLFoneAccountConnectionErrorWindow::~CUOLFoneAccountConnectionErrorWindow() 
{
}


void CUOLFoneAccountConnectionErrorWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneAccountConnectionErrorWindow::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneAccountConnectionErrorWindow::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneAccountConnectionErrorWindow::SetChildWindowInfo(CUOLFoneChildWindowInfo childWindowInfo)
{
	CString strTitle = childWindowInfo.GetTitle();
	
	if (!strTitle.IsEmpty())
	{
		m_statusPanel.SetStatusString(strTitle);
	}
}


LRESULT CUOLFoneAccountConnectionErrorWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls...
	//
	
	m_userPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	m_lineSeparator.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_centerFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_statusPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	ATLASSERT(m_userPanel.IsWindow());
	ATLASSERT(m_lineSeparator.IsWindow());
	ATLASSERT(m_centerFiller.IsWindow());
	ATLASSERT(m_statusPanel.IsWindow());
	
	//
	// Building layout...
	//
	
	// Build line 1.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_userPanel, 
			m_userPanel.GetWndClassName(), 
			m_userPanel.GetMinimumWindowWidth(), 0, 
			m_userPanel.GetMinimumWindowHeight(), TRUE);
	
	// Add line 2.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_lineSeparator, 
			m_lineSeparator.GetWndClassName(), 20, 0, 
			COMPONENTS_VERTICAL_SEPARATION, TRUE);
	
	// Build line 3.
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, TRUE);
	m_componentsOrganizer.AddComponent(&m_centerFiller, 
			m_centerFiller.GetWndClassName(), 40, 0, COMPONENT_DEFAULT_HEIGHT, TRUE);
	
	// Build line 4.
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_statusPanel, 
			m_statusPanel.GetWndClassName(), m_statusPanel.GetMinimumWindowWidth(), 
			0, m_statusPanel.GetMinimumWindowHeight(), TRUE);
	
	return 0;
}


LRESULT CUOLFoneAccountConnectionErrorWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_userPanel.DestroyWindow();
	m_lineSeparator.DestroyWindow();
	m_centerFiller.DestroyWindow();
	m_statusPanel.DestroyWindow();
	
	return 0;
}


LRESULT CUOLFoneAccountConnectionErrorWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorWindow::OnShowWindow(UINT /*uMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	BOOL bShow=(BOOL)wShow;
	
	if (bShow)
	{
		m_userPanel.UpdateWindowData();
	}
	
	return 0L;
}


LRESULT CUOLFoneAccountConnectionErrorWindow::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

