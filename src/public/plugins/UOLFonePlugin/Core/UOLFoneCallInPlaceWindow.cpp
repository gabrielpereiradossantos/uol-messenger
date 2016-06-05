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
#include ".\UOLFoneCallInPlaceWindow.h"

#include "../Resource.h"
#include "../FacadeWrapper.h"
#include "../UOLFonePluginTheme.h"
#include "UOLFoneController.h"



/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow positioning defines
/////////////////////////////////////////////////////////////////////


#define		DEFAULT_SHIFT						0

#define		LEFT_SHIFT							DEFAULT_SHIFT
#define		TOP_SHIFT							DEFAULT_SHIFT
#define		RIGHT_SHIFT							DEFAULT_SHIFT
#define		BOTTOM_SHIFT						DEFAULT_SHIFT

#define		COMPONENT_DEFAULT_HEIGHT			25
#define		HIGHER_STATIC_SIZE					20
#define		SLIDER_DEFAULT_WIDTH				68
#define		COMPONENTS_HORIZONTAL_SEPARATION	10
#define		COMPONENTS_VERTICAL_SEPARATION		2
#define		BLOCK_SEPARATION					5

#define		CONTACT_IMAGE_WIDTH					46
#define		CONTACT_IMAGE_HEIGHT				46




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallInPlaceUserPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneCallInPlaceUserPanel::CUOLFoneCallInPlaceUserPanel(void)
{
	m_backgroundColor = RGB(0xFF, 0xFF, 0xFF);
}


CUOLFoneCallInPlaceUserPanel::~CUOLFoneCallInPlaceUserPanel(void)
{
}


LRESULT CUOLFoneCallInPlaceUserPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetBackgroundColor(m_backgroundColor);
	
	//
	// Creating controls ...
	//
	
	m_userNameFont.CreatePointFont(80, "Tahoma", 0, true);
	
	// Somente um filler sobre o qual serah desenhada a imagem do contato.
	m_callUserImage.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_columnSeparator.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_callUserName.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_WORDELLIPSIS);
	m_callUserName.SetFont(m_userNameFont);
	
	ATLASSERT(m_callUserImage.IsWindow());
	ATLASSERT(m_columnSeparator.IsWindow());
	ATLASSERT(m_callUserName.IsWindow());
	
	// Building layout ...
	
	AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_CENTERED);
	AddComponent(&m_callUserImage, 
			m_callUserImage.GetWndClassName(), 
			CONTACT_IMAGE_WIDTH, 0, CONTACT_IMAGE_HEIGHT);
	AddComponent(&m_columnSeparator, 
			m_columnSeparator.GetWndClassName(), 
			COMPONENTS_HORIZONTAL_SEPARATION, 0, CONTACT_IMAGE_HEIGHT);
	AddComponent(&m_callUserName, 
			m_callUserName.GetWndClassName(), 40, 0, 0, TRUE);
	
	return 0L;
}


LRESULT CUOLFoneCallInPlaceUserPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_callUserImage.DestroyWindow();
	m_columnSeparator.DestroyWindow();
	m_callUserName.DestroyWindow();
	
	m_userNameFont.DeleteObject();
	
	return 0L;
}


LRESULT CUOLFoneCallInPlaceUserPanel::OnPaint(HDC hDc)
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
			IUOLMessengerUIManager::IDS_DRAW_OPAQUE_STRETCHED);

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


LRESULT CUOLFoneCallInPlaceUserPanel::OnEraseBkgnd(HDC hDc)
{
	CRect imageRect;
	GetComponentClientPosition(m_callUserImage, imageRect);
	
	::ExcludeClipRect(hDc, imageRect.left, imageRect.top, imageRect.right, imageRect.bottom);
	
	return __super::OnEraseBackground(hDc);
}


void CUOLFoneCallInPlaceUserPanel::UpdateWindowData()
{	
	m_callUserName.SetWindowText( CFacadeWrapper::GetInstance()->GetVoIPCallText() );
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallInPlaceStatusPanel
/////////////////////////////////////////////////////////////////////


CUOLFoneCallInPlaceStatusPanel::CUOLFoneCallInPlaceStatusPanel(void)
{
	m_backgroundColor = RGB(0xD4, 0xDF, 0x82);
	m_textColor = RGB(0x0A, 0x58, 0x0A);
}


CUOLFoneCallInPlaceStatusPanel::~CUOLFoneCallInPlaceStatusPanel(void)
{
}


void CUOLFoneCallInPlaceStatusPanel::SetStatusString(LPCTSTR statusStr)
{
	m_callInPlaceText.SetWindowText(statusStr);
	OrganizeComponents();
}


LRESULT CUOLFoneCallInPlaceStatusPanel::OnCreate(
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
	m_infoTextFont.CreatePointFont(70, "Tahoma");
	
	m_callInPlaceBitmap.LoadBitmap(IDB_ICO_SUCCESS);
	
	m_callInPlaceImage.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_BITMAP);
	m_callInPlaceImage.SetBitmap(m_callInPlaceBitmap.m_hBitmap);
	
	labelStr.LoadString(IDS_LABEL_LIGACAO_EM_ANDAMENTO);
	m_callInPlaceText.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_callInPlaceText.SetFont(m_textFont);
	
	labelStr.LoadString(IDS_INFO_SEND_TONS_WITH_KEYBOARD);
	m_infoText.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER);
	m_infoText.SetFont(m_infoTextFont);
	m_infoText.SetText(labelStr);
	
	ATLASSERT(m_callInPlaceImage.IsWindow());
	ATLASSERT(m_callInPlaceText.IsWindow());
	ATLASSERT(m_infoText.IsWindow());
	
	// Building layout ...
	
	// Build line 1.
	AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	AddComponent(&m_callInPlaceImage, m_callInPlaceImage.GetWndClassName(), 
			15);
	AddComponent(&m_callInPlaceText, m_callInPlaceText.GetWndClassName());
	
	// Add a separator line.
	AddSeparatorLine(5);
	
	// Build line 3.
	AddLine();
	AddComponent(&m_infoText, m_infoText.GetWndClassName(), 
			m_infoText.GetWidth(), 0, 
			m_infoText.GetHeight(), TRUE);
	
	return 0L;
}


LRESULT CUOLFoneCallInPlaceStatusPanel::OnDestroy(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_callInPlaceImage.DestroyWindow();
	m_callInPlaceText.DestroyWindow();
	m_infoText.DestroyWindow();
	
	m_textFont.DeleteObject();
	m_infoTextFont.DeleteObject();
	
	m_callInPlaceBitmap.DeleteObject();
	
	return 0L;
}


LRESULT CUOLFoneCallInPlaceStatusPanel::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnSize(uMsg, wParam, lParam, bHandled);
	
	m_callInPlaceText.Invalidate();
	
	return 0L;
}




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallInPlaceWindow
/////////////////////////////////////////////////////////////////////


CUOLFoneCallInPlaceWindow::CUOLFoneCallInPlaceWindow() : 
		m_showingWindow(FALSE),
		m_bIsSpeakerMute(FALSE),
		m_bIsMicrophoneMute(FALSE)
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);
	
	m_pUOLFoneController = CUOLFoneController::GetInstance();
}


CUOLFoneCallInPlaceWindow::~CUOLFoneCallInPlaceWindow() 
{
	m_FoneKeyboardCtrl.UnregisterObserver(this);
}


void CUOLFoneCallInPlaceWindow::GetMinSize(CSize& size)
{
	size.cx = m_componentsOrganizer.GetMinimumWindowWidth();
	size.cy = m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneCallInPlaceWindow::UpdateAccountInfo(CUOLFoneAccountPtr pAccount)
{
}


void CUOLFoneCallInPlaceWindow::UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall)
{
}


void CUOLFoneCallInPlaceWindow::SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo)
{
	// Do something with the information.
}


BOOL CUOLFoneCallInPlaceWindow::CanGetFocus()
{
	return FALSE;
}


void CUOLFoneCallInPlaceWindow::ResetWindow()
{
}



/////////////////////////////////////////////////////////////////////
// Map messages handling
/////////////////////////////////////////////////////////////////////


LRESULT CUOLFoneCallInPlaceWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{	
	//
	// First of all, let's initialize the MIXER components, so whether 
	// we have some problem, we shut down the window.
	//
	
	m_dwSpeakerMinimum = 0;
	m_dwSpeakerMaximum = 100;
	m_dwSpeakerVolumeControlID = 0;
	
	m_dwMicrophoneMinimum = 0;
	m_dwMicrophoneMaximum = 100;
	m_dwMicrophoneVolumeControlID = 0;
	
	m_dwSpeakerMuteControlID = 0;
	m_dwMicrophoneMuteControlID = 0;

	m_dwLastMicLevel = 0;
	m_dwLastSpkLevel = 0;
	
	//
	// Creating controls...
	//
	
	CString labelStr;
	
	m_sectionFont.CreatePointFont(80, "Tahoma", 0, true);
	
	m_userPanel.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
	
	m_microphoneLevel.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | TBS_HORZ | TBS_BOTTOM, 
			0, ID_SLIDER_MICROPHONE);
	m_microphoneLevel.SetBackgroundColor(RGB(0xD4, 0xDF, 0x82));
	m_microphoneLevel.SetNotifyingWindow(m_hWnd);
	
	m_speakerLevel.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | TBS_HORZ | TBS_BOTTOM, 
			0, ID_SLIDER_SPEAKER);
	m_speakerLevel.SetBackgroundColor(RGB(0xD4, 0xDF, 0x82));
	m_speakerLevel.SetNotifyingWindow(m_hWnd);
		
	m_sidesFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_middleFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	m_centerFiller.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW);
	
	labelStr.LoadString(IDS_LABEL_SPEAKER);
	m_btnSpeaker.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_SPEAKER);
	m_multiStateSpeaker.SubclassWindow(m_btnSpeaker.m_hWnd);
	m_multiStateSpeaker.SetMultiStateBitmap(IDB_BTN_SPEAKER_ON);
	m_multiStateSpeaker.SetCharForwardingWnd(m_hWnd);

	labelStr.LoadString(IDS_LABEL_MICROPHONE);
	m_btnMicrophone.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_MICROPHONE);
	m_multiStateMicrophone.SubclassWindow(m_btnMicrophone.m_hWnd);
	m_multiStateMicrophone.SetMultiStateBitmap(IDB_BTN_MICROPHONE_ON);
	m_multiStateMicrophone.SetCharForwardingWnd(m_hWnd);

	m_FoneKeyboardCtrl.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, 
			WS_CHILDWINDOW | WS_VISIBLE);
	m_FoneKeyboardCtrl.RegisterObserver(this);
	
	ATLASSERT(m_userPanel.IsWindow());
	ATLASSERT(m_microphoneLevel.IsWindow());
	ATLASSERT(m_speakerLevel.IsWindow());
	ATLASSERT(m_sidesFiller.IsWindow());
	ATLASSERT(m_middleFiller.IsWindow());
	ATLASSERT(m_centerFiller.IsWindow());
	ATLASSERT(m_btnSpeaker.IsWindow());
	ATLASSERT(m_btnMicrophone.IsWindow());
	ATLASSERT(m_FoneKeyboardCtrl.IsWindow());
	
	//
	// Building layout...
	//
	
	// User panel line
	m_componentsOrganizer.AddLine();
	m_componentsOrganizer.AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 10);
	m_componentsOrganizer.AddComponent(&m_userPanel, 
			m_userPanel.GetWndClassName(), 
			m_userPanel.GetMinimumWindowWidth(), 0, 
			m_userPanel.GetMinimumWindowHeight(), TRUE);
	m_componentsOrganizer.AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 10);
	
	// Add a separator line.
	m_componentsOrganizer.AddSeparatorLine(BLOCK_SEPARATION);

	// Virtual keyboard line
	m_componentsOrganizer.AddLine(0, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_FoneKeyboardCtrl, 
			m_FoneKeyboardCtrl.GetWndClassName(), 
			m_FoneKeyboardCtrl.GetWidth(), 0, m_FoneKeyboardCtrl.GetHeight());
	
	// Add a separator line.
	m_componentsOrganizer.AddSeparatorLine(COMPONENTS_VERTICAL_SEPARATION);
	
	// Microphone and Speaker controls line
	m_componentsOrganizer.AddLine(5, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED, 
			CWTLWndComponentsOrganizer::LINE_VORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_sidesFiller, 
			m_sidesFiller.GetWndClassName(), 5);
	m_componentsOrganizer.AddComponent(&m_btnSpeaker, 
			m_btnSpeaker.GetWndClassName(), 12, 12, 12);
	m_componentsOrganizer.AddComponent(&m_speakerLevel, 
			m_speakerLevel.GetWndClassName(), SLIDER_DEFAULT_WIDTH, 
			20, 20);
	m_componentsOrganizer.AddComponent(&m_middleFiller, 
		m_middleFiller.GetWndClassName(), 5, 5, 5, TRUE);
	m_componentsOrganizer.AddComponent(&m_btnMicrophone, 
			m_btnMicrophone.GetWndClassName(), 12, 12, 12);
	m_componentsOrganizer.AddComponent(&m_microphoneLevel, 
			m_microphoneLevel.GetWndClassName(), SLIDER_DEFAULT_WIDTH, 
			20, 20);
	CFacadeWrapper::GetInstance()->RegisterAsMainWindowEventsNotifier(this);
		
	return 0;
}


LRESULT CUOLFoneCallInPlaceWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFacadeWrapper::GetInstance()->UnregisterAsMainWindowEventsNotifier(this);
	
	m_userPanel.DestroyWindow();
	m_microphoneLevel.DestroyWindow();
	m_speakerLevel.DestroyWindow();
	m_sidesFiller.DestroyWindow();
	m_middleFiller.DestroyWindow();
	m_centerFiller.DestroyWindow();
	m_FoneKeyboardCtrl.DestroyWindow();
	
	m_sectionFont.DeleteObject();
	
	return 0;
}


LRESULT CUOLFoneCallInPlaceWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	return 0L;
}


LRESULT CUOLFoneCallInPlaceWindow::OnShowWindow(UINT /*uMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_showingWindow = (BOOL)wShow;
	
	DWORD dwValue = 0;
	LONG lValue = 0;
	
	if (m_showingWindow) {
		
		if (GetSystemMicrophoneVolumeLevel(dwValue)) {
			
			m_microphoneLevel.SetRange(m_dwMicrophoneMinimum, m_dwMicrophoneMaximum / 10);
			m_dwLastMicLevel = dwValue / 10;
			m_microphoneLevel.SetPos(m_dwLastMicLevel);
		}
		
		dwValue = 0;
		if (GetSystemSpeakersVolumeLevel(dwValue)) {
			
			m_speakerLevel.SetRange(m_dwSpeakerMinimum, m_dwSpeakerMaximum / 10);
			m_dwLastSpkLevel = dwValue / 10;
			m_speakerLevel.SetPos(m_dwLastSpkLevel);
		}
		
		if (GetSystemMicrophoneMuteValue(lValue)) 
		{
			m_bIsMicrophoneMute = (BOOL)lValue;
			ChangeMicrophoneButtonImage(m_bIsMicrophoneMute);
		}
		
		lValue = 0;
		if (GetSystemSpeakersMuteValue(lValue)) 
		{
			m_bIsSpeakerMute = (BOOL)lValue;
			ChangeSpeakerButtonImage(m_bIsSpeakerMute);
		}
		
		m_userPanel.UpdateWindowData();
	}
	
	return 0L;
}

LRESULT CUOLFoneCallInPlaceWindow::OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	int		nScrollCode = (int)LOWORD(wParam);
    int		nPos = (short int)HIWORD(wParam);
	HWND	hWnd = (HWND)lParam;
	
	if (hWnd == m_microphoneLevel.m_hWnd) 
	{	
		DWORD dwCurPos = m_microphoneLevel.GetPos();
		if ( dwCurPos != m_dwLastMicLevel)
		{
			m_dwLastMicLevel = dwCurPos;
			
			ATLASSERT(m_microphoneLevel.IsWindowEnabled());

			SetSystemMicrophoneVolumeLevel(dwCurPos * 10);
		}
	}
	else if (hWnd == m_speakerLevel.m_hWnd) 
	{
		DWORD dwCurPos = m_speakerLevel.GetPos();
		if ( dwCurPos != m_dwLastSpkLevel)
		{
			m_dwLastSpkLevel = dwCurPos;
			
			ATLASSERT(m_speakerLevel.IsWindowEnabled());

			SetSystemSpeakersVolumeLevel(dwCurPos * 10);
		}
	}
	
	return 0L;
}


void CUOLFoneCallInPlaceWindow::OnButtonClicked(UINT /*uCode*/, int nID, HWND hWndCtrl)
{
	switch (nID)
	{
	case ID_BUTTON_SPEAKER:
		m_bIsSpeakerMute = !m_bIsSpeakerMute;
		SetSystemSpeakersMuteValue(m_bIsSpeakerMute);
		ChangeSpeakerButtonImage(m_bIsSpeakerMute);
		break;
	case ID_BUTTON_MICROPHONE:
		m_bIsMicrophoneMute = !m_bIsMicrophoneMute;
		SetSystemMicrophoneMuteValue(m_bIsMicrophoneMute);
		ChangeMicrophoneButtonImage(m_bIsMicrophoneMute);
		break;
	}
}


LRESULT CUOLFoneCallInPlaceWindow::OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


LRESULT CUOLFoneCallInPlaceWindow::OnCharEventNotifier(
		UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HandleKeyboardInput((UINT)wParam);
	
	return 0L;
}


/////////////////////////////////////////////////////////////////////
// Volume control methods
/////////////////////////////////////////////////////////////////////

BOOL CUOLFoneCallInPlaceWindow::GetSystemMicrophoneVolumeLevel(DWORD &dwVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	dwVal = m_pUOLFoneController->GetCurrentInputDeviceVolume();
	
	return TRUE;
}


BOOL CUOLFoneCallInPlaceWindow::GetSystemSpeakersVolumeLevel(DWORD &dwVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	dwVal = m_pUOLFoneController->GetCurrentOutputDeviceVolume();
	
	return TRUE;
}


BOOL CUOLFoneCallInPlaceWindow::SetSystemMicrophoneVolumeLevel(DWORD dwVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	
	if (!m_bIsMicrophoneMute)
	{
		m_pUOLFoneController->SetCurrentInputDeviceVolume(dwVal);
	}
	
	return TRUE;
}


BOOL CUOLFoneCallInPlaceWindow::SetSystemSpeakersVolumeLevel(DWORD dwVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	m_pUOLFoneController->SetCurrentOutputDeviceVolume(dwVal);
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////
// Mute control methods
/////////////////////////////////////////////////////////////////////


BOOL CUOLFoneCallInPlaceWindow::GetSystemMicrophoneMuteValue(LONG &lVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	lVal = m_pUOLFoneController->GetCurrentInputDeviceMute();
	
	return TRUE;
}


BOOL CUOLFoneCallInPlaceWindow::GetSystemSpeakersMuteValue(LONG &lVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	lVal = m_pUOLFoneController->GetCurrentOutputDeviceMute();

	return TRUE;
}


BOOL CUOLFoneCallInPlaceWindow::SetSystemMicrophoneMuteValue(LONG lVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	m_pUOLFoneController->SetCurrentInputDeviceMute(lVal);
	
	return TRUE;
}


BOOL CUOLFoneCallInPlaceWindow::SetSystemSpeakersMuteValue(LONG lVal) const
{
	ATLASSERT(NULL != m_pUOLFoneController);
	m_pUOLFoneController->SetCurrentOutputDeviceMute(lVal);

	return TRUE;
}


void CUOLFoneCallInPlaceWindow::OnMainWindowChar(UINT nChar)
{
	if (m_showingWindow)
	{
		HandleKeyboardInput(nChar);
	}
}


void CUOLFoneCallInPlaceWindow::HandleKeyboardInput(UINT nChar, BOOL bPressFoneKeyboardKey)
{
	/*
	CString strMsg;
	strMsg.Format("CUOLFoneCallInPlaceWindow::HandleKeyboardInput(%d)", nChar);
	
	MessageBox(strMsg);
	*/
	if (bPressFoneKeyboardKey)
	{
		m_FoneKeyboardCtrl.PressFoneKeyboardKey(nChar);
	}

	CFacadeWrapper::GetInstance()->PlaySound(nChar);
	
	CUOLFoneController::GetInstance()->PlayTone(nChar);
}


void CUOLFoneCallInPlaceWindow::OnFoneKeyboardPressed(UINT nChar, BOOL bCleanLast)
{
	HandleKeyboardInput(nChar, FALSE);
}


void CUOLFoneCallInPlaceWindow::ChangeMicrophoneButtonImage(BOOL bIsMute)
{
	if (bIsMute)
	{
		m_multiStateMicrophone.SetMultiStateBitmap(IDB_BTN_MICROPHONE_OFF);
	}
	else
	{
		m_multiStateMicrophone.SetMultiStateBitmap(IDB_BTN_MICROPHONE_ON);
	}
}


void CUOLFoneCallInPlaceWindow::ChangeSpeakerButtonImage(BOOL bIsMute)
{
	if (bIsMute)
	{
		m_multiStateSpeaker.SetMultiStateBitmap(IDB_BTN_SPEAKER_OFF);
	}
	else
	{
		m_multiStateSpeaker.SetMultiStateBitmap(IDB_BTN_SPEAKER_ON);
	}
}