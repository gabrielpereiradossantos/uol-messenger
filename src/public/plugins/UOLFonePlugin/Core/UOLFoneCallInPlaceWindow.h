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


#include <interfaces/IUOLMessengerMainWindowEventsNotifier.h>

#include "WTLWndComponentsOrganizer.h"
#include "IUOLMessengerPluginCustomTabChildWindow.h"
#include "UOLFoneOrganizedColoredPanel.h"
#include "UOLFoneSlider.h"
#include "UOLFoneChildWindowInfo.h"
#include "ButtonWMEventNotifier.h"
#include "AutoResizeStatic.h"
#include "UOLFoneKeyboardCtrl.h"

class CUOLFoneController;




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallInPlaceUserPanel
/////////////////////////////////////////////////////////////////////


class CUOLFoneCallInPlaceUserPanel : public CUOLFoneOrganizedColoredPanel
{
public:
	
	CUOLFoneCallInPlaceUserPanel(void);
	virtual ~CUOLFoneCallInPlaceUserPanel(void);
	
	void UpdateWindowData();
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneCallInPlaceUserPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(HDC hDc);
	LRESULT OnEraseBkgnd(HDC hDc);
	
private:
	
	WTL::CStatic	m_callUserImage;
	WTL::CStatic	m_columnSeparator;
	WTL::CStatic	m_callUserName;
	
	WTL::CFont		m_userNameFont;
	
	COLORREF		m_backgroundColor;
	COLORREF		m_textColor;
};




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallInPlaceStatusPanel
/////////////////////////////////////////////////////////////////////


class CUOLFoneCallInPlaceStatusPanel : public CUOLFoneOrganizedColoredPanel
{
public:
	
	CUOLFoneCallInPlaceStatusPanel(void);
	virtual ~CUOLFoneCallInPlaceStatusPanel(void);
	
	void SetStatusString(LPCTSTR statusStr);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneCallInPlaceStatusPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
private:
	
	WTL::CStatic		m_callInPlaceImage;
	WTL::CStatic		m_callInPlaceText;
	CAutoResizeStatic	m_infoText;
	
	WTL::CBitmap		m_callInPlaceBitmap;
	
	WTL::CFont			m_textFont;
	WTL::CFont			m_infoTextFont;
	
	COLORREF			m_backgroundColor;
	COLORREF			m_textColor;
};




/////////////////////////////////////////////////////////////////////
// CUOLFoneCallInPlaceWindow
/////////////////////////////////////////////////////////////////////


class CUOLFoneCallInPlaceWindow;

typedef CWinTraits<WS_CHILD | WS_CLIPCHILDREN, WS_EX_TOPMOST> CTopMostControlWinTraits;


class CUOLFoneCallInPlaceWindow : 
		public CWindowImpl<CUOLFoneCallInPlaceWindow, CWindow, CTopMostControlWinTraits>,
		public IUOLMessengerPluginCustomTabChildWindow, 
		public CUOLMessengerMainWindowEventsObserver,
		public CUOLFoneKeyboardCtrlObserver
{
public:
	
	DECLARE_WND_CLASS(NULL)
	
	CUOLFoneCallInPlaceWindow();
	virtual ~CUOLFoneCallInPlaceWindow();
	
	virtual void GetMinSize(CSize& size);
	virtual void UpdateAccountInfo(CUOLFoneAccountPtr pAccount);
	virtual void UpdatePhoneCallInfo(CPhoneCallPtr pPhoneCall);
	virtual void SetChildWindowInfo(CUOLFoneChildWindowInfo* pChildWindowInfo);
	virtual BOOL CanGetFocus();
	virtual void ResetWindow();

	// CUOLFoneKeyboardCtrlObserver
	virtual void OnFoneKeyboardPressed(UINT nChar, BOOL bCleanLast);
	
protected:
	enum
	{
		ID_CHECK_BUTTON_MICROPHONE = 0x2344,
		ID_CHECK_BUTTON_SOUND,
		ID_SLIDER_MICROPHONE,
		ID_SLIDER_SPEAKER,
		ID_BUTTON_HANG_CALL,
		ID_BUTTON_SPEAKER,
		ID_BUTTON_MICROPHONE
	};
	
	BEGIN_MSG_MAP(CUOLFoneCallInPlaceWindow57)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		COMMAND_RANGE_HANDLER_EX(ID_BUTTON_SPEAKER, ID_BUTTON_MICROPHONE, OnButtonClicked)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, OnCtlColor)
		MESSAGE_HANDLER(WM_ON_CHAR_EVENT_NOTIFIER, OnCharEventNotifier)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnMicrophoneCheckClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSoundCheckClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnButtonClicked(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	LRESULT OnCtlColor(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCharEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	BOOL	GetMicrophoneVolumeControl();
	BOOL	GetSpeakersVolumeControl();
	
	BOOL	GetSystemMicrophoneVolumeLevel(DWORD &dwVal) const;
	BOOL	GetSystemSpeakersVolumeLevel(DWORD &dwVal) const;
	
	BOOL	SetSystemMicrophoneVolumeLevel(DWORD dwVal) const;
	BOOL	SetSystemSpeakersVolumeLevel(DWORD dwVal) const;
	
	BOOL	GetMicrophoneMuteControl();
	BOOL	GetSpeakersMuteControl();
	
	BOOL	GetSystemMicrophoneMuteValue(LONG &lVal) const;
	BOOL	GetSystemSpeakersMuteValue(LONG &lVal) const;
	
	BOOL	SetSystemMicrophoneMuteValue(LONG lVal) const;
	BOOL	SetSystemSpeakersMuteValue(LONG lVal) const;
	
	virtual void OnMainWindowChar(UINT nChar);
	
	void HandleKeyboardInput(UINT nChar, BOOL bPressFoneKeyboardKey = TRUE);
	void ChangeMicrophoneButtonImage(BOOL bIsMute);
	void ChangeSpeakerButtonImage(BOOL bIsMute);
	
private:
	
	// Interface elements.
	CUOLFoneCallInPlaceUserPanel	m_userPanel;
	
	CUOLFoneSlider	m_microphoneLevel;
	CUOLFoneSlider	m_speakerLevel;
	
	WTL::CStatic	m_sidesFiller;
	WTL::CStatic	m_middleFiller;
	WTL::CStatic	m_centerFiller;
	
	WTL::CFont		m_sectionFont;
	
	CWTLWndComponentsOrganizer	m_componentsOrganizer;
	
	DWORD	m_dwSpeakerMinimum;
	DWORD	m_dwSpeakerMaximum;
	DWORD	m_dwSpeakerVolumeControlID;
	
	DWORD	m_dwMicrophoneMinimum;
	DWORD	m_dwMicrophoneMaximum;
	DWORD	m_dwMicrophoneVolumeControlID;
	
	DWORD	m_dwSpeakerMuteControlID;
	DWORD	m_dwMicrophoneMuteControlID;

	CUOLFoneController*		m_pUOLFoneController;

	DWORD	m_dwLastMicLevel;
	DWORD	m_dwLastSpkLevel;
	
	BOOL	m_showingWindow;

	CUOLFoneKeyboardCtrl		m_FoneKeyboardCtrl;

	BOOL						m_bIsSpeakerMute;
	BOOL						m_bIsMicrophoneMute;
	WTL::CButton				m_btnSpeaker;
	WTL::CButton				m_btnMicrophone;
	CUOLFoneMultiStateButton	m_multiStateSpeaker;
	CUOLFoneMultiStateButton	m_multiStateMicrophone;
};

MAKEAUTOPTR(CUOLFoneCallInPlaceWindow);

