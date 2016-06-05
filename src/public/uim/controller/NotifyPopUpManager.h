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

#include <interfaces/IUOLMessengerNotifyPopUpManager.h>
#include "../view/NotifyPopUp/NotifyPopUp.h"


class CNotifyPopUpManager: 
	public IUOLMessengerNotifyPopUpManager,
	public CNotifyPopUpObserver,
	public CWindowImpl<CNotifyPopUpManager, CWindow>
{
public:
	DECLARE_WND_CLASS("NotifyPopUpWndController")

	CNotifyPopUpManager();
	virtual ~CNotifyPopUpManager();

	virtual BOOL Init();
	virtual void Finalize();

protected:
	virtual void ShowAlertWindow(const CString& strTitle, 
								 CAtlList<CString>& listTextLines, 
								 IUOLMessengerImagePtr pMessageIcon = IUOLMessengerImagePtr(),  
								 IUOLMessengerNotifyPopUpHyperLinkHandlerPtr pTextLinkHandler = IUOLMessengerNotifyPopUpHyperLinkHandlerPtr(),
								 IUOLMessengerNotifyPopUpHyperLinkHandlerPtr pOptionsLinkHandler = IUOLMessengerNotifyPopUpHyperLinkHandlerPtr(),
								 IUOLMessengerImagePtr pWindowIcon = IUOLMessengerImagePtr(),
								 DWORD dwShowTimeout = SHOW_TIMEOUT_DEFAULT);
	virtual void ShowAlertWindow(IUOLMessengerNotifyPopUpHandlerPtr pNotifyPopUpHandler, DWORD dwShowTimeout = SHOW_TIMEOUT_DEFAULT);

	virtual void OnClose(CNotifyPopUp* pNotifyPopUp);
	virtual void OnOptions(CNotifyPopUp* pNotifyPopUp);
	virtual void OnClick(CNotifyPopUp* pNotifyPopUp);


	BEGIN_MSG_MAP(CNotifyPopUpManager)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(CNotifyPopUp::WM_CHILD_POPUP_CLICKED, OnChildClicked)
		MESSAGE_HANDLER(CNotifyPopUp::WM_POPUP_MOUSE_ENTER, OnPopUpMouseEnter)
		MESSAGE_HANDLER(CNotifyPopUp::WM_POPUP_MOUSE_LEAVE, OnPopUpMouseLeave)
	END_MSG_MAP()

private:
	CNotifyPopUpPtr CreateNotifyPopUp();
	void Add(CNotifyPopUpPtr pNotifyPopUp);

	BOOL CloseChild(CNotifyPopUp* pNotifyPopUp);
	BOOL ProtectChild(CNotifyPopUpPtr pNotifyPopUp);

	void ShowChildren();
	void UpdateChildren();
	void DeleteOldChildren();

	void OnTimer(UINT nID, TIMERPROC tp);
	void OnDestroy();

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	LRESULT OnChildClicked(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPopUpMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPopUpMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CComAutoCriticalSection					m_criticalSection;

	CAtlList<CNotifyPopUpPtr>				m_listPopUps;

	UINT_PTR								m_nIDTimer;
	INT										m_nTimerInterval;
	DWORD									m_dwTimeout;
	BOOL									m_bIsDirty;
	BOOL									m_bFirstTimer;
	BOOL									m_bStopTimer;
	DWORD									m_dwTickCount;
};
MAKEAUTOPTR(CNotifyPopUpManager);