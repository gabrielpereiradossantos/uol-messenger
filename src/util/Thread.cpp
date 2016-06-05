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
#include "memory.h"
#include "Thread.h"


class CAutoEventSetter
{
public:
	CAutoEventSetter (CEvent& event) :
	   m_autoEvent(event)
	{
	}

	~CAutoEventSetter ()
	{
		m_autoEvent.Set();
	}

private:
	   CEvent& m_autoEvent;

};

CThread::CThread(IRunnable* pRunnable) : 
	m_pRunnable(pRunnable),
	m_hThread(NULL),
	m_nThreadID(0), 
	m_nStackSize(0)
{
	m_eventThread.Create(NULL, TRUE, FALSE, NULL);
}

CThread::~CThread(void)
{
	if (m_hThread)
	{
		::CloseHandle(m_hThread);
	}
}


BOOL CThread::Start()
{	
	if (IsAlive())
	{
		ATLASSERT(FALSE);

		return FALSE;
	}		

	m_hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, m_nStackSize, ThreadFunc, this, 0, &m_nThreadID));
	
	if (m_hThread != NULL)
	{
		::WaitForSingleObject(m_eventThread, INFINITE);
	}
	
	return m_hThread != NULL;
}


BOOL CThread::Wait(DWORD dwMilliseconds) const
{
	ATLASSERT(::GetCurrentThreadId() != m_nThreadID);

	// Evita deadlock
	if (::GetCurrentThreadId() == m_nThreadID)
	{
		return FALSE;
	}

	return ::WaitForSingleObject(m_hThread, dwMilliseconds) == WAIT_OBJECT_0;
}

BOOL CThread::MessageLoopWait(DWORD dwMilliseconds) const
{	
	ATLASSERT(::GetCurrentThreadId() != m_nThreadID);
	
	// We are not interested on this thread's messages.
	//
	if ((::GetCurrentThreadId() == m_nThreadID) ||
			(NULL == m_hThread))
	{
		return FALSE;
	}
	
	DWORD dwEndTime = (dwMilliseconds != INFINITE) ? (::GetCurrentTime() + dwMilliseconds) : INFINITE;
	
	// Processa o loop de msgs enquanto a thread _Execute estiver viva.
	//
	do
	{
		DWORD dwWait = (dwEndTime != INFINITE) ? (dwEndTime - ::GetTickCount()) : INFINITE;
		MSG wndMsg;
		
		if ( (INFINITE != dwWait) && (static_cast<LONG>(dwWait) <= 0) )
		{
			//CAppLogger::GetInstance(FALSE)->Log(logLevelDebug, "CThread::MessageLoopWait() - TIMEOUT!\n");
			return FALSE;
		}
		
		// Bloca enquanto não existir msgs na fila ou enquato m_hThread ainda estiver viva.
		//
		switch (MsgWaitForMultipleObjectsEx(1, &m_hThread,
				dwWait, QS_ALLINPUT, MWMO_INPUTAVAILABLE))
		{
			case WAIT_OBJECT_0:
				// Thread has ended...
				// Just break that it will leave the do-while loop below.
				break;
			
			case (WAIT_OBJECT_0 + 1):
				
				if (::PeekMessage(&wndMsg, NULL, 0, 0, PM_REMOVE))
				{
					if (wndMsg.message == WM_QUIT)
					{				
						PostQuitMessage((int)wndMsg.wParam);
						
						return FALSE;
					}
					
					::TranslateMessage(&wndMsg);
					::DispatchMessage(&wndMsg);	
				}
				
				break;
				
			default:
				// Unexpected failure.
				//
				return FALSE;
		}
	}
	while (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThread, WAIT_OBJECT_0));
	
	return TRUE;
}

void CThread::SetStackSize(unsigned nStackSize)
{
	m_nStackSize = nStackSize;
}

unsigned CThread::GetThreadID() const
{
	return m_nThreadID;
}

HANDLE CThread::GetThreadHandle() const
{
	return m_hThread;
}

BOOL CThread::IsAlive() const
{
	return (WAIT_TIMEOUT == ::WaitForSingleObject(m_hThread, WAIT_OBJECT_0));
}

BOOL CThread::SetThreadPriority(int nPriority)
{
	if (IsAlive())
	{
		return ::SetThreadPriority(m_hThread, nPriority);
	}

	return FALSE;
}

int  CThread::GetThreadPriority(int nPriority) const 
{
	if (IsAlive())
	{
		return ::GetThreadPriority(m_hThread);
	}

	return THREAD_PRIORITY_ERROR_RETURN;
}

void CThread::SetName(LPCTSTR szThreadName)
{
	m_strThreadName = szThreadName;

	if (IsAlive())
	{
		SetThreadName(szThreadName, m_nThreadID);
	}
}

void CThread::SetThreadName(LPCSTR szThreadName, DWORD dwThreadID)
{
	struct THREAD_NAME_INFO
	{
		DWORD	dwType;
		LPCSTR	szName;
		DWORD   dwThreadID;
		DWORD	dwFlags;
	};

	THREAD_NAME_INFO tni;

	tni.dwType = 0x1000;
	tni.szName = szThreadName;
	tni.dwThreadID = dwThreadID;
	tni.dwFlags = 0;

	__try
	{
		::RaiseException(0x406d1388, 0, 
			sizeof(THREAD_NAME_INFO) / sizeof(DWORD), 
			(DWORD*)&tni);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

void CThread::Run()
{
	ATLASSERT(FALSE);
}

CThread& CThread::operator=(const CThread&)
{
	ATLASSERT(FALSE);

	return *this;
}

unsigned  __stdcall CThread::ThreadFunc(void* lpParam)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	IRunnable* pRunnable = NULL;

	{
		CThread*  pThread = reinterpret_cast<CThread*>(lpParam);

		CAutoEventSetter eventSet(pThread->m_eventThread);

		if (FALSE == pThread->m_strThreadName.IsEmpty())
		{
			SetThreadName(pThread->m_strThreadName);
		}

		pRunnable = pThread->m_pRunnable ? pThread->m_pRunnable : pThread;

		if (::IsBadReadPtr(pRunnable))
		{
			ATLASSERT(FALSE);
			return (unsigned)-1;
		}

	}
	
	if (pRunnable != NULL)
	{
		pRunnable->Run();	
		return 0L;
	}	

	return 0L;
}

