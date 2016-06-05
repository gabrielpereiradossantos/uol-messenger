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


#include "stdafx.h"
#include "SessionTimerWindow.h"

using namespace std;

typedef struct _TIMERMESSAGEPARAM
{
	std::string strUser;
	std::string strContact;
	ITimerWindowCallback* pCallback;

	_TIMERMESSAGEPARAM& operator= (const _TIMERMESSAGEPARAM& tmp)
	{
		strUser = tmp.strUser;
		strContact = tmp.strContact;

		return *this;
	}
} TIMERMESSAGEPARAM, *LPTIMERMESSAGEPARAM;



typedef struct _SYNCHRONIZEIMPARAM
{
	std::string strParam1;
	std::string strParam2;
	std::string strParam3;
	ISynchronizeIMCallback* pSynchronizeCallback;

} SYNCHRONIZEIMPARAM, *LPSYNCHRONIZEIMPARAM;



CSessionTimerWindow::CSessionTimerWindow()
{
}


CSessionTimerWindow::~CSessionTimerWindow()
{
}


void CSessionTimerWindow::Init()
{
	Create(::GetDesktopWindow());

	if (IsWindow())
	{
		ShowWindow(SW_HIDE);
	}
}



void CSessionTimerWindow::Finalize()
{
	if (IsWindow())
	{
		DestroyWindow();
	}

	CTimerWindow* pTimerWindow;

	CTimerWindowMap::iterator it;
	for (it = m_mapTimerWindow.begin(); it != m_mapTimerWindow.end(); it++)
	{
		pTimerWindow = it->second;

		if (pTimerWindow)
		{
			if (pTimerWindow->IsWindow())
			{
				pTimerWindow->DestroyWindow();
			}
			else
			{
				delete pTimerWindow;
			}
		}
	}

}


void CSessionTimerWindow::StartTimer(const std::string& strUser, const std::string& strContact, ITimerWindowCallback* pCallback)
{
	if (IsWindow())
	{
		TIMERMESSAGEPARAM tmp;
		tmp.strUser = strUser;
		tmp.strContact = strContact;
		tmp.pCallback = pCallback;

		SendMessage(WM_USER_START_TIMER, (WPARAM) &tmp, NULL);
	}
}


void CSessionTimerWindow::StopTimer(const std::string& strUser, const std::string& strContact)
{
	if (IsWindow())
	{
		TIMERMESSAGEPARAM tmp;
		tmp.strUser = strUser;
		tmp.strContact = strContact;
		tmp.pCallback = NULL;

		SendMessage(WM_USER_STOP_TIMER, (WPARAM) &tmp, NULL);
	}
}


void CSessionTimerWindow::SynchronizeIMReceived(const std::string& strToUser, 
		const std::string& strFromUser, const std::string& strText, 
		ISynchronizeIMCallback* pSynchronizeCallback)
{
	if (IsWindow())
	{
		SYNCHRONIZEIMPARAM* pSyncParam = new SYNCHRONIZEIMPARAM;
		
		if (pSyncParam)
		{
			pSyncParam->strParam1 = strToUser;
			pSyncParam->strParam2 = strFromUser;
			pSyncParam->strParam3 = strText;
			pSyncParam->pSynchronizeCallback = pSynchronizeCallback;
			
			// This invocation couldn't be blocking.
			//
			PostMessage(WM_USER_SYNC_IM_RECEIVED, (WPARAM)pSyncParam, NULL);
		}
	}
}


void CSessionTimerWindow::SynchronizeIMSent(const std::string& strFromUser, 
		const std::string& strToUser, const std::string& strText, 
		ISynchronizeIMCallback* pSynchronizeCallback)
{
	if (IsWindow())
	{
		SYNCHRONIZEIMPARAM* pSyncParam = new SYNCHRONIZEIMPARAM;
		
		if (pSyncParam)
		{
			pSyncParam->strParam1 = strFromUser;
			pSyncParam->strParam2 = strToUser;
			pSyncParam->strParam3 = strText;
			pSyncParam->pSynchronizeCallback = pSynchronizeCallback;
			
			// This invocation couldn't be blocking.
			//
			PostMessage(WM_USER_SYNC_IM_SENT, (WPARAM)pSyncParam, NULL);
		}
	}
}


LRESULT CSessionTimerWindow::OnStartTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPTIMERMESSAGEPARAM pTmp = (LPTIMERMESSAGEPARAM) wParam;

	if (pTmp)
	{
		string strSessionId = GetTimerWindowId(pTmp->strUser, pTmp->strContact);

		CTimerWindowMap::iterator it = m_mapTimerWindow.find(strSessionId);

		if (it != m_mapTimerWindow.end())
		{
			CTimerWindow* pOldTimerWindow = it->second;

			pOldTimerWindow->SetCallback(NULL);

			bool bDestroyed = pOldTimerWindow->DestroyWindow();
			ATLASSERT(bDestroyed);

			// If destroy window succeeds, CTimerWindow will be deleted
			// otherwise, avoid memory leak
			if (!bDestroyed)
			{
				delete pOldTimerWindow;
			}

			m_mapTimerWindow.erase(it);
		}

		CTimerWindow* pTimerWindow = new CTimerWindow(pTmp->strUser, pTmp->strContact);

		pTimerWindow->Create(::GetDesktopWindow());

		if (pTimerWindow->IsWindow())
		{
			pTimerWindow->ShowWindow(SW_HIDE);
			//pTimerWindow->SetWindowLongPtr(GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));

			m_mapTimerWindow[strSessionId] = pTimerWindow;

			pTimerWindow->SetCallback(pTmp->pCallback);
			pTimerWindow->Start();
		}
		else
		{
			delete pTimerWindow;
		}
	}

	return 0L;
}


LRESULT CSessionTimerWindow::OnStopTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPTIMERMESSAGEPARAM pTmp = (LPTIMERMESSAGEPARAM) wParam;

	if (pTmp)
	{
		string strSessionId = GetTimerWindowId(pTmp->strUser, pTmp->strContact);

		CTimerWindowMap::iterator it = m_mapTimerWindow.find(strSessionId);

		if (it != m_mapTimerWindow.end())
		{
			CTimerWindow* pTimerWindow = it->second;

			pTimerWindow->SetCallback(NULL);
			pTimerWindow->Stop();

			bool bDestroyed = pTimerWindow->DestroyWindow();
			ATLASSERT(bDestroyed);

			if (!bDestroyed)
			{
				delete pTimerWindow;
			}

			m_mapTimerWindow.erase(it);
		}
	}

	return 0L;
}


LRESULT CSessionTimerWindow::OnSyncIMReceived(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPSYNCHRONIZEIMPARAM pSyncParam = (LPSYNCHRONIZEIMPARAM)wParam;
	
	if ( (pSyncParam) && (pSyncParam->pSynchronizeCallback) )
	{
		pSyncParam->pSynchronizeCallback->OnSyncIMReceived(
				pSyncParam->strParam1, pSyncParam->strParam2, pSyncParam->strParam3);
		
		delete pSyncParam;
	}
	
	return 0L;
}


LRESULT CSessionTimerWindow::OnSyncIMSent(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPSYNCHRONIZEIMPARAM pSyncParam = (LPSYNCHRONIZEIMPARAM)wParam;
	
	if ( (pSyncParam) && (pSyncParam->pSynchronizeCallback) )
	{
		pSyncParam->pSynchronizeCallback->OnSyncIMSent(
				pSyncParam->strParam1, pSyncParam->strParam2, pSyncParam->strParam3);
		
		delete pSyncParam;
	}
	
	return 0L;
}


std::string CSessionTimerWindow::GetTimerWindowId(const std::string& strUser, const std::string& strContact)
{
	return strContact + "\\" + strUser;
}