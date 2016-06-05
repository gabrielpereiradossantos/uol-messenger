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

#include "RadioUOLView.h"
#include "SongListMng.h"

#include <interfaces/IUOLMessengerPluginCustomPanel.h>
#include "../RadioTooltipProvider.h"
#include "htmlView.h"
#include "RadioUOLMng.h"

class CRadioUOLWindow;

#define MAX_POPUP_ITENS 1024

class CRadioUOLWindow : 
	public CWindowImpl<CRadioUOLWindow> ,
	public IUOLMessengerPluginCustomPanel,
	public CRadioUOLMng ,
	public CSongListObserver
{
public:
	DECLARE_WND_CLASS(NULL)

	CRadioUOLWindow(const CString& strPanelName, const CString& strPluginInternalName);

	BOOL PreTranslateMessage(MSG* pMsg);

	enum eCommands {
		NOTIFY_TRAY = MAX_POPUP_ITENS,
		NOTIFY_CHANNEL,
		SHOW_CHANNELS
	};

	enum wndMessages
	{
		WM_CREATE_RADIO_SYSTRAY_MENU = WM_USER + 0x432,
		WM_DESTROY_RADIO_SYSTRAY_MENU
	};

	BEGIN_MSG_MAP(CRadioUOLWindow22)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnErase)
		COMMAND_RANGE_HANDLER(0, (MAX_POPUP_ITENS-1), OnPopupSelected)
		COMMAND_ID_HANDLER(NOTIFY_TRAY, OnNotifyTray)
		COMMAND_ID_HANDLER(SHOW_CHANNELS, OnShowChannels)
		MESSAGE_HANDLER_EX(WM_CREATE_RADIO_SYSTRAY_MENU, OnCreateRadioSystrayMenu)
		MESSAGE_HANDLER_EX(WM_DESTROY_RADIO_SYSTRAY_MENU, OnDestroyRadioSystrayMenu)
	END_MSG_MAP()

	virtual void OnBuy();
	virtual void OnSearch();
	virtual void OnChannels();
	virtual void OnStyles(CPoint pt);
	virtual void OnMute();
	
	//CSongListObserver
	virtual void OnLoadedChannels(int, int);
	virtual void OnLoadedWMPURL(int) ;
	virtual void OnLoadedSongTitle(int);
	virtual void OnLoadedFerrugem(int);

	void SetConfig(CRadioConfig *config);

	// IUOLMessengerPluginCustomPanel start
	CString GetPanelName() const;
	CString GetPluginInternalName() const;
	void GetMinSize(CSize& size);
	void DestroyPanel(HWND hWndPanel);
	HWND CreatePanel(HWND hWndParent, const CRect& rectPanel, DWORD dwWindowStyle = WS_CHILDWINDOW);
	IUOLMessengerTooltipProviderPtr GetTooltipProvider() const;
	
	//  Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	LRESULT OnCreateRadioSystrayMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnDestroyRadioSystrayMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	void PlayerPlay();
	void PlayerPause();
	void PlayerStop();
	void PlayerPrevious();
	void PlayerNext();

	CAtlList<CStyle*> & GetChannels();

	void PlaySelectedChannel(int iChannelId);

private:
	void ShowNotify(void);
	UINT_PTR		m_timer;
	BOOL m_bLoadedConfig;
	BOOL m_bLoadedChannels;

	CString		m_strPanelName;
	CString		m_strPluginInternalName;
	CRadioTooltipProviderPtr m_pTooltipProvider;
	CPoint		m_ptMouse;

public:
	LRESULT OnErase(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPopupSelected(UINT uMsg, WPARAM wParam, HWND /*hwnd*/, BOOL& bHandled);
	LRESULT OnNotifyTray(UINT /*uMsg*/, WPARAM /*wParam*/, HWND /*hwnd*/, BOOL& /*bHandled*/);
	LRESULT OnShowChannels(UINT /*uMsg*/, WPARAM /*wParam*/, HWND /*hwnd*/, BOOL& /*bHandled*/);
};

MAKEAUTOPTR(CRadioUOLWindow);