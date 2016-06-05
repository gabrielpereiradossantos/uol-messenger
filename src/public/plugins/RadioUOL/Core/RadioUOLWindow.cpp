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
#include ".\radiouolwindow.h"

#define TAMANHO_MENU 45
#define TAMANHO_WMP 32
#define TIMER_ID 1234
#define TIMER_TEST_ID 4321

#include <shellapi.h>
#include "RequestTicket.h"
#include "RequestChannels.h"
#include "RequestWMPURL.h"
#include "ChannelsPopupMenu.h"

#include "../facadeWrapper.h"
#include "../RadioNotifyPopupHyperLinkHandler.h"
#include "wininet.h"
#include "../resource.h"
//CRadioUOLMng* CRadioUOLWindow::m_mng = NULL;
//CRadioUOLWindow* g_ptr = NULL;


/////////////////////////////////////////////////////////////////////
// CRadioUOLWindow
/////////////////////////////////////////////////////////////////////

CRadioUOLWindow::CRadioUOLWindow(const CString& strPanelName, const CString& strPluginInternalName):
	m_strPanelName(strPanelName),
	m_strPluginInternalName(strPluginInternalName),
	m_timer(0),
	m_bLoadedConfig(FALSE),
	m_bLoadedChannels(FALSE)
{
	m_pTooltipProvider = new CRadioTooltipProvider(m_strPanelName);
	//m_mng = this;
}


void CRadioUOLWindow::ShowNotify(void)
{
	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetFacadeInstance();
	IUOLMessengerNotifyPopUpManagerPtr pNotifyPopUpManager = pFacade->GetNotifyPopUpManager();
	IUOLMessengerUIManagerPtr uiManager;

	uiManager = pFacade->GetUIManager();
	if ((uiManager!=NULL) && (m_songs != NULL))
	{
		CWindow wndAppWindow = uiManager->GetAppWindow();

		HWND wndCtl = ::GetFocus();

		if (wndAppWindow.IsChild(wndCtl))
		{
			return ;
		}

		CString artist;
		CString songName;
		CAtlList<CString> listLines;

		CString strText;
		BOOL flag = FALSE;

		if (m_songs->GetArtistName().IsEmpty()==FALSE) 
		{
			strText.Format(_T("Artista: %s"), m_songs->GetArtistName());
			flag  = TRUE;

			listLines.AddTail(strText);
		}

		if (m_songs->GetSongName().IsEmpty()==FALSE)
		{
			strText.Format(_T("Música: %s"), m_songs->GetSongName());
			flag  = TRUE;

			listLines.AddTail(strText);
		}

		if (flag)
		{
			IUOLMessengerImagePtr pWindowIcon = uiManager->LoadImage(ATL::_AtlBaseModule.GetModuleInstance(), IDB_NOTIFICATION);

			CString strText;
			strText.LoadString(IDS_RADIO_NOTIFICATION);
			pNotifyPopUpManager->ShowAlertWindow(strText, 
				listLines, m_songs->GetSongImage(), new CRadioNotifyPopUpHyperLinkHandler(), 
				IUOLMessengerNotifyPopUpHyperLinkHandlerPtr(),
				pWindowIcon);
		}

	}

}

BOOL CRadioUOLWindow::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}


LRESULT CRadioUOLWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_songs = new CSongListMng;
	ATLASSERT(m_songs!=NULL);
	m_songs->FireChannelsThread(FALSE);

	ATLASSERT(m_pConfig!=NULL);

	if (m_songs->RegisterObserver(this)==FALSE)
	{
		ATLASSERT(FALSE);
	}

	AtlAxWinInit();

	CComPtr<IAxWinHostWindow>  spHost;
	CRect rcWMP(10,300,150,342);
	CRect rect(1,1,200,50);
	HRESULT  hr;
	HWND hwnd;


	hwnd = m_uolView.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	ATLASSERT(hwnd!=NULL);

	// Criar o clock para atualizar a musica e duracao
	m_timer = (UINT_PTR) SetTimer(TIMER_ID,1000);
	ATLASSERT (m_timer!=NULL);

	// Cria a janela que ira conter o WMP
	hwnd = m_wndView.Create(m_hWnd, rcWMP, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	ATLASSERT(hwnd!=NULL);

	hr = m_wndView.QueryHost(&spHost);
	ATLASSERT(hr==S_OK);

	if ((SUCCEEDED(hr)) && (spHost != NULL))
	{
		// Cria o controle WMP
		hr = spHost->CreateControl(CComBSTR(_T("{6BF52A52-394A-11d3-B153-00C04F79FAA6}")), m_wndView, 0);
		ATLASSERT(hr==S_OK);

		if (SUCCEEDED(hr))
		{
			// Obtem o ponteiro para a interface que controla o player
			hr = m_wndView.QueryControl(&m_spPlayer);
			ATLASSERT(hr==S_OK);

			if ((SUCCEEDED(hr)) && (m_spPlayer != NULL))
			{
				m_spPlayer->put_uiMode(CComBSTR("full"));

				// Obtem a interface de controle do WMP
				m_spPlayer->get_controls(&m_pC);

				DispEventAdvise(m_spPlayer);
			}
		}
	}

	return 0;
}

LRESULT CRadioUOLWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_pFrameWindow!=NULL)
	{
		if (m_pFrameWindow->GetHWND())
		{
			CWindow wndFrameWindow = m_pFrameWindow->GetHWND();
			wndFrameWindow.DestroyWindow();
		}
	}

	// Destroi o clock da atualizacao da musica
	if (m_timer!=0)
	{
		KillTimer(m_timer);
		m_timer = 0;
	}

	// Destroi o WMP 
	if(m_spPlayer)
	{
		// Unhook the event listener
		HRESULT hr;
		hr = DispEventUnadvise(m_spPlayer);
		hr = m_spPlayer->close();
		m_spPlayer.Release();
		m_pC.Release();
	}

	if (m_wndView.IsWindow())
	{
		m_wndView.DestroyWindow();
	}

	if (m_songs!=NULL)
	{
		m_songs->UnRegisterObserver(this);

		delete m_songs;
		m_songs = NULL;
	}

	return 0;
}

LRESULT CRadioUOLWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rect;
	GetClientRect(&rect);
	CRect tmpRect(rect);
	tmpRect.bottom = TAMANHO_MENU ;
	if (m_uolView.IsWindow())
	{
		m_uolView.MoveWindow(tmpRect);
	}
	tmpRect.top = TAMANHO_MENU ;
	tmpRect.bottom = TAMANHO_MENU + TAMANHO_WMP;
	if (m_wndView.IsWindow())
	{
		m_wndView.MoveWindow(tmpRect);
	}
	return 0;
}



LRESULT CRadioUOLWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam == TIMER_ID)
	{
		UpdateDisplay();
	} 
	return 0;
}




LRESULT CRadioUOLWindow::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,RGB(0,255,0));
	return 0;
}


void CRadioUOLWindow::OnBuy()
{
	BOOL bConnected;
	DWORD dwFlags;
	bConnected = InternetGetConnectedState ( &dwFlags, 0 );
	if ( bConnected )
	    if ( dwFlags & INTERNET_CONNECTION_OFFLINE )
		  bConnected = FALSE;

	if (bConnected)
	{
		CString strText;
		int result = m_songs->GetResult(strText);
		if (result>0)
		{
			ShellExecute(NULL, "open", strText, "", "", SW_SHOW);
		}
	} 
	else
	{
		CRadioUOLError error(CRadioUOLError::INTERNET_OFF);
	}

}

void CRadioUOLWindow::OnMute()
{
}

void CRadioUOLWindow::OnSearch()
{
	BOOL bConnected;
	DWORD dwFlags;
	bConnected = InternetGetConnectedState ( &dwFlags, 0 );
	if ( bConnected )
	    if ( dwFlags & INTERNET_CONNECTION_OFFLINE )
		  bConnected = FALSE;
	
	if (bConnected)
	{
		ShowSearch(1);
	} 
	else
	{
		CRadioUOLError error(CRadioUOLError::INTERNET_OFF);
	}
}

void CRadioUOLWindow::OnChannels()
{
	BOOL bConnected;
	DWORD dwFlags;
	bConnected = InternetGetConnectedState ( &dwFlags, 0 );
	if ( bConnected )
	    if ( dwFlags & INTERNET_CONNECTION_OFFLINE )
		  bConnected = FALSE;
	
	if (bConnected)
	{
		ShowSearch(2);
	} 
	else
	{
		CRadioUOLError error(CRadioUOLError::INTERNET_OFF);
	}
}

void CRadioUOLWindow::OnStyles(CPoint pt)
{
	BOOL bConnected;
	DWORD dwFlags;
	bConnected = InternetGetConnectedState ( &dwFlags, 0 );
	if ( bConnected )
	    if ( dwFlags & INTERNET_CONNECTION_OFFLINE )
		  bConnected = FALSE;
	
	if (bConnected)
	{
		m_ptMouse = pt;
		if (m_bLoadedChannels==FALSE)
		{
			if (m_songs->FireChannelsThread()==FALSE)
			{
				ATLTRACE("Thread already launched!\n");
			}
		}
		else 
		{
			PostMessage(WM_COMMAND, SHOW_CHANNELS);
		}
	} 
	else
	{
		CRadioUOLError error(CRadioUOLError::INTERNET_OFF);
	}
}

void CRadioUOLWindow::OnLoadedChannels(int result, int bNotify)
{
	ATLTRACE("%s\n",__FUNCTION__);
	
	if (result==0)
	{
		::PostMessage(m_hWnd, WM_CREATE_RADIO_SYSTRAY_MENU, 0, 0);
		
		if (bNotify)
		{
			m_bLoadedChannels = TRUE;
			PostMessage(WM_COMMAND, SHOW_CHANNELS);
		}
	} 
	else
	{
		CRadioUOLError error(result);
	}
}


void CRadioUOLWindow::OnLoadedWMPURL(int result)
{

	if (result == 0)
	{
		CString tmpString = m_songs->GetWMPURL();
		if (tmpString.IsEmpty()==FALSE)
		{
			ATLTRACE("%s %s\n",__FUNCTION__, tmpString);
			if (m_spPlayer!=NULL)
			{
				m_spPlayer->put_URL(CComBSTR(tmpString));
			}
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
	else
	{
		CRadioUOLError error(result);
	}

}

void CRadioUOLWindow::OnLoadedFerrugem(BOOL result)
{
	ATLTRACE("%s\n",__FUNCTION__);
}

void CRadioUOLWindow::OnLoadedSongTitle(BOOL result)
{
	ATLTRACE("%s\n",__FUNCTION__);
	if (m_songs)
	{
		m_pTooltipProvider->SetArtist(m_songs->GetArtistName());
		m_pTooltipProvider->SetSongName(m_songs->GetSongName());
		m_pTooltipProvider->SetCdName(m_songs->GetCdName());
		m_pTooltipProvider->SetNextSongName(m_songs->GetNextSongName());
		m_pTooltipProvider->SetImage(m_songs->GetSongImage());
	}

	UpdateDisplay();
	
	if (result==0)
	{
		PostMessage(WM_COMMAND, NOTIFY_TRAY);
	}
}

LRESULT CRadioUOLWindow::OnPopupSelected(UINT uMsg, WPARAM wParam, HWND lParam, BOOL& /*bHandled*/)
{
	ATLTRACE("%s\n",__FUNCTION__);
	int id = LOWORD(wParam) /*- WM_USER*/;
	
	PlaySelectedChannel(id);
	
	return S_OK;
}




LRESULT CRadioUOLWindow::OnErase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled=FALSE;
	return 0;
}


// IUOLMessengerPluginCustomPanel start

CString CRadioUOLWindow::GetPanelName() const
{
	return m_strPanelName;
}


CString CRadioUOLWindow::GetPluginInternalName() const
{
	return m_strPluginInternalName;
}


void CRadioUOLWindow::GetMinSize(CSize& size)
{
	size.cx = 0;
	size.cy = 77;
}


HWND CRadioUOLWindow::CreatePanel(HWND hWndParent, const CRect& rectPanel, DWORD dwWindowStyle)
{
	CRect rectWindow = rectPanel;

	HWND hWnd = Create(hWndParent, rectWindow, NULL, dwWindowStyle);

	return hWnd;
}


void CRadioUOLWindow::DestroyPanel(HWND hWndPanel)
{
	if (hWndPanel == m_hWnd)
	{
		DestroyWindow();
	}
}


IUOLMessengerTooltipProviderPtr CRadioUOLWindow::GetTooltipProvider() const
{
	WMPPlayState state;
	if (m_spPlayer!=NULL)
	{
		m_spPlayer->get_playState(&state);
		switch (state ) {
			case wmppsPlaying:
			case wmppsPaused:
			case wmppsStopped:
				return m_pTooltipProvider;
		}
	}
	return NULL;
}


LRESULT CRadioUOLWindow::OnCreateRadioSystrayMenu(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CFacadeWrapper::GetInstance()->BuildRadioSystrayMenu();
	
	return 0L;
}


LRESULT CRadioUOLWindow::OnDestroyRadioSystrayMenu(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CFacadeWrapper::GetInstance()->ClearRadioSystrayMenu();
	
	return 0L;
}


void CRadioUOLWindow::PlayerPlay()
{
	m_pC->play();
}


void CRadioUOLWindow::PlayerPause()
{
	m_pC->pause();
}


void CRadioUOLWindow::PlayerStop()
{
	m_pC->stop();
}


void CRadioUOLWindow::PlayerPrevious()
{
	m_pC->previous();
}


void CRadioUOLWindow::PlayerNext()
{
	m_pC->next();
}


CAtlList<CStyle*> & CRadioUOLWindow::GetChannels()
{
	return m_songs->GetStyle();
}


void CRadioUOLWindow::PlaySelectedChannel(int iChannelId)
{
	ATLASSERT(iChannelId >= 0);
	ATLASSERT(m_songs);
	
	if (m_songs)
	{
		m_songs->FireURLThread(iChannelId);
	}
}


LRESULT CRadioUOLWindow::OnNotifyTray(UINT /*uMsg*/, WPARAM /*wParam*/, HWND /*hwnd*/, BOOL& /*bHandled*/)
{
	ShowNotify();
	return 0;
}


LRESULT CRadioUOLWindow::OnShowChannels(UINT /*uMsg*/, WPARAM /*wParam*/, HWND /*hwnd*/, BOOL& /*bHandled*/)
{
	CRect rect;
	GetWindowRect(rect);
	CChannelsPopupMenu popup(m_songs->GetStyle());
	popup.Show(m_hWnd, m_ptMouse.x+rect.left, m_ptMouse.y+rect.top);	
	return 0;
}


void CRadioUOLWindow::SetConfig(CRadioConfig *config)
{
	m_pConfig = config;
}
