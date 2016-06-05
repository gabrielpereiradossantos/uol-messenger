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
#include ".\messageloopthread.h"

#include "ThreadPriority.h"
#include "processHelper.h"

CMessageLoopThread::CMessageLoopThread(void)
{
}

CMessageLoopThread::~CMessageLoopThread(void)
{
}

BOOL CMessageLoopThread::Start()
{
	if (__super::Start())
	{
		PostThreadMessage(WM_NULL);

		return TRUE;
	}
	
	return FALSE;
}

void CMessageLoopThread::Stop()
{
	if (m_nThreadID != 0)
	{
		::PostThreadMessage(m_nThreadID, WM_QUIT, 0, 0);	
	}
}

BOOL CMessageLoopThread::AddMessageFilter(CMessageFilter* pMessageFilter)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	return CMessageLoop::AddMessageFilter(pMessageFilter);
}

BOOL CMessageLoopThread::RemoveMessageFilter(CMessageFilter* pMessageFilter)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	return CMessageLoop::RemoveMessageFilter(pMessageFilter);
}
	
BOOL CMessageLoopThread::AddIdleHandler(CIdleHandler* pIdleHandler)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	return CMessageLoop::AddIdleHandler(pIdleHandler);
}
BOOL CMessageLoopThread::RemoveIdleHandler(CIdleHandler* pIdleHandler)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	return CMessageLoop::RemoveIdleHandler(pIdleHandler);
}

BOOL CMessageLoopThread::PreTranslateMessage(MSG* pMsg)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	return CMessageLoop::PreTranslateMessage(pMsg);
}

BOOL CMessageLoopThread::OnIdle(int nIdleCount)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	return CMessageLoop::OnIdle(nIdleCount);
}

void CMessageLoopThread::Run()
{
	//CAppLogger::GetInstance(FALSE)->Log(logLevelDebug, "[%d] - Entering CMessageLoopThread::Run()...\n", GetThreadID());

	CMessageLoop::Run();

	//CAppLogger::GetInstance(FALSE)->Log(logLevelDebug, "[%d] - Leaving CMessageLoopThread::Run().\n", GetThreadID());
}


BOOL CMessageLoopThread::PostThreadMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CThreadPriority tp(THREAD_PRIORITY_BELOW_NORMAL);
	const int SPIN_COUNT = 4000;

	for (int nInd = 0; nInd < 2; nInd++)
	{
		if (1 == nInd)
		{
			::Sleep(500);
		}

		for (int nSpinCount = 0;  nSpinCount < SPIN_COUNT; nSpinCount++)
		{
			// PostThreadMessage retorna FALSE enquanto m_thread não criar um fila de msgs.
			if (::PostThreadMessage(GetThreadID(), uMsg, wParam, lParam))
			{
				if (nSpinCount > (SPIN_COUNT / 10))
				{
					//CAppLogger::GetInstance(FALSE)->Log(logLevelDebug, "CMessageLoopThread::PostThreadMessage() - returns TRUE - %d spins.\n", nSpinCount);
				}

				return TRUE;
			}

			::DelayExecution();
		}			
	}	


	//CAppLogger::GetInstance(FALSE)->Log(logLevelDebug, "CMessageLoopThread::PostThreadMessage() - returns FALSE\n");
	ATLASSERT(FALSE);

	return FALSE;
}



