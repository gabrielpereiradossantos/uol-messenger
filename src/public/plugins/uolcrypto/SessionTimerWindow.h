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

#include "TimerWindow.h"


class ISynchronizeIMCallback
{
public:
	virtual ~ISynchronizeIMCallback()
	{
	}
	
	virtual void OnSyncIMReceived(const std::string& strToUser, const std::string& strFromUser, const std::string& strText) = 0;
	virtual void OnSyncIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText) = 0;
};


class CSessionTimerWindow : public CWindowImpl<CSessionTimerWindow, CWindow, CNullTraits>
{
public:
	CSessionTimerWindow();
	~CSessionTimerWindow();

	DECLARE_WND_CLASS("UIMCryptoSessionTimerWindow");

	void Init();
	void Finalize();

	void StartTimer(const std::string& strUser, const std::string& strContact, ITimerWindowCallback* pCallback);
	void StopTimer(const std::string& strUser, const std::string& strContact);

	void SynchronizeIMReceived(const std::string& strToUser, const std::string& strFromUser, 
			const std::string& strText, ISynchronizeIMCallback* pSynchronizeCallback);
	void SynchronizeIMSent(const std::string& strFromUser, const std::string& strToUser, 
			const std::string& strText, ISynchronizeIMCallback* pSynchronizeCallback);

protected:
	enum
	{
		WM_USER_START_TIMER = WM_USER + 0x031,
		WM_USER_STOP_TIMER,
		WM_USER_SYNC_IM_RECEIVED,
		WM_USER_SYNC_IM_SENT
	};

	BEGIN_MSG_MAP(CSessionTimerWindow)
		MESSAGE_HANDLER(WM_USER_START_TIMER, OnStartTimer)
		MESSAGE_HANDLER(WM_USER_STOP_TIMER, OnStopTimer)
		MESSAGE_HANDLER(WM_USER_SYNC_IM_RECEIVED, OnSyncIMReceived)
		MESSAGE_HANDLER(WM_USER_SYNC_IM_SENT, OnSyncIMSent)
	END_MSG_MAP()

	LRESULT OnStartTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnStopTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSyncIMReceived(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSyncIMSent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	std::string GetTimerWindowId(const std::string& strUser, const std::string& strContact);

private:
	typedef std::map<std::string, CTimerWindow*>	CTimerWindowMap;

	CTimerWindowMap		m_mapTimerWindow;
};