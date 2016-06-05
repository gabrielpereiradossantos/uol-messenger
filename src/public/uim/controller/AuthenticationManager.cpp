#include "StdAfx.h"
#include "AuthenticationManager.h"
#include <atlutil.h>

#define SLEEP_TIMEOUT				1000*5
#define SLEEP_REAUTHENTICATION		1000*15*60
//#define SLEEP_DEVEL					1000*7

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CAuthenticationManager::CAuthenticationManager() :
m_strCookie (""),
m_hMyThread(0),
m_hMyThreadID(0),
m_hThreadAuth(0),
m_hMyThreaAuthdID(0)
{
	//ATLTRACE("%s\n",__FUNCTION__);

	m_authResult = CInternetAuthentication::AUTH_FALSE;

	m_bReAuthenticate = FALSE;

	m_hAuthenticationEvent = CreateEvent( 
		NULL,         // default security attributes
		FALSE,        // manual-reset event
		FALSE,        // initial state is signaled
		"ExecuteAuthentication"  // object name
		); 

	if (m_hAuthenticationEvent == NULL) 
	{ 
		ATLTRACE("CreateEvent failed (%d)\n", GetLastError());
		ATLASSERT(FALSE);
	}
}



CAuthenticationManager::~CAuthenticationManager()
{
	//ATLTRACE("%s START \n",__FUNCTION__);

	if (m_hThreadAuth) 
	{
		WaitForSingleObject(m_hThreadAuth,500);
	}

	if (m_hMyThread) 
	{
		m_bReAuthenticate = FALSE;

		ExecuteAuthenticationNow();

		WaitForSingleObject(m_hMyThread,500);
	}

	if (m_hAuthenticationEvent!=NULL) 
	{
		CloseHandle(m_hAuthenticationEvent);
		m_hAuthenticationEvent = NULL;
	}

	//ATLTRACE("%s END \n",__FUNCTION__);
}

void CAuthenticationManager::StopReAuthentication()
{
	m_bReAuthenticate = FALSE;
}

BOOL CAuthenticationManager::DoAuthentication(const CString userlogin, const CString password, BOOL reauthenticate)
{
	m_user = userlogin;

	m_password = password;

	m_bReAuthenticate = reauthenticate;

	if (m_hMyThread==0)
	{
		if (InitAuthenticationThread()==FALSE) 
			return FALSE;
	} 
	else
	{
		ExecuteAuthenticationNow();
	}

	return TRUE;
}


BOOL CAuthenticationManager::InitAuthenticationThread()
{
	ATLTRACE("%s\n",__FUNCTION__);

	if (m_hMyThread!=0)
	{
		ATLTRACE("%s WARNING Thread already lunched  \n",__FUNCTION__);	

		return FALSE;
	}

	m_hMyThread = (HANDLE) _beginthreadex(NULL,0,MyAuthenticationThread	,this ,0 ,&m_hMyThreadID);

	if (m_hMyThread) 
	{
		//ATLTRACE("%s Thread ID %d lunched \n",__FUNCTION__, m_hMyThreadID);	

	}
	else
	{
		ATLTRACE("%s WARNING Thread NOT lunched\n",__FUNCTION__);	

		return FALSE;
	} 

	return TRUE;
}

UINT WINAPI CAuthenticationManager::MyAuthenticationThread(LPVOID pParam)
{
	CAuthenticationManager * pItem ;

	pItem = static_cast <CAuthenticationManager *> (pParam);

	pItem->MyAuthenticationThreadFunction();

	CloseHandle(pItem->m_hMyThread);

	pItem->m_hMyThread = 0;

	return 0;
}


void CAuthenticationManager::MyAuthenticationThreadFunction()
{
	//ATLTRACE("%s <START> \n",__FUNCTION__);

	while (1) 
	{
		//ATLTRACE("%s Try2ing Auth %d h %d ###\n",__FUNCTION__,m_hThreadAuth, m_hThreadAuth);
		if (m_hThreadAuth==0)
		{
			m_hThreadAuth = (HANDLE) _beginthreadex(NULL,0,StartAuthenticationThread,this,0 ,&m_hMyThreaAuthdID);

			if (m_hThreadAuth) 	
			{
				ATLTRACE("%s Thread ID %d lunched \n",__FUNCTION__, m_hMyThreaAuthdID);	
			}
			else
			{
				ATLTRACE("%s WARNING Thread NOT lunched\n",__FUNCTION__);	

				return ;
			} 
			switch(WaitForSingleObject(m_hThreadAuth,SLEEP_TIMEOUT))
			{
			case WAIT_TIMEOUT:

				ATLTRACE("%s WARNING TIMEOUT \n",__FUNCTION__);	

				NotifyAll(&CAuthenticationObserver::OnAuthenticate,"",CInternetAuthentication::AUTH_CONN_ERROR);

				break;
			}

		}

		//ATLTRACE("%s WaitForSingleObject START \n",__FUNCTION__);
		
		if (m_bReAuthenticate==FALSE)
		{
			break;
		}

		WaitForSingleObject(m_hAuthenticationEvent, SLEEP_REAUTHENTICATION);

		if (m_bReAuthenticate==FALSE)
		{
			break;
		}

		//ATLTRACE("%s WaitForSingleObject END \n",__FUNCTION__);



	}
	ATLTRACE("%s <END> \n",__FUNCTION__);
}


//////////////////////////////////////////////////////////////////////////


void CAuthenticationManager::RegisterObserver(CAuthenticationObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);
		if (pos == NULL)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}

void CAuthenticationManager::UnregisterObserver(CAuthenticationObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);
		if (pos != NULL)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}


void CAuthenticationManager::NotifyAll(void (CAuthenticationObserver::*pfnHandler)(const CString & cookie, LONG status),
									   const CString cookie,
									   LONG status)
{
	//ATLTRACE("%s  cookie %s status %d\n",__FUNCTION__,cookie,status);
	CAuthenticationObserver* pObserver;
	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		pObserver = m_listObservers.GetNext(pos);
		if (FALSE == ::IsBadReadPtr(pObserver, sizeof(CAuthenticationObserver*)))
		{
			(pObserver->*pfnHandler)(m_strCookie,  m_authResult);
		}
		else
		{
			ATLASSERT(FALSE);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////


UINT WINAPI CAuthenticationManager::StartAuthenticationThread(LPVOID pParam)
{
	ATLTRACE("%s -----> START \n",__FUNCTION__);

	CAuthenticationManager * pItem ;

	pItem = static_cast <CAuthenticationManager *> (pParam);

	//ATLTRACE("ESPERANDO INICIO\n");

	//Sleep(1000*60);

	//ATLTRACE("ESPERANDO FIM\n");

	pItem->CallAuthentication();

#ifdef SLEEP_DEVEL
	Sleep(SLEEP_DEVEL);
#endif

	CloseHandle(pItem->m_hThreadAuth);

	pItem->m_hThreadAuth = 0;

	ATLTRACE("%s -----> END \n",__FUNCTION__);

	return 0;
}

void CAuthenticationManager::CallAuthentication()
{
	CAuthentication *authentication;

	authentication = new CAuthentication;

	if (authentication==NULL)
	{
		ATLASSERT(FALSE);
	}

	m_authResult = authentication->DoAuthentication(m_user, m_password, m_strCookie );	

	//ATLTRACE("\nuser:%s\nAuthentication return:%d\ncookie %s\n" ,m_user, m_authResult, m_strCookie);

	NotifyAll(&CAuthenticationObserver::OnAuthenticate,m_strCookie,(LONG) m_authResult);

	delete authentication;
}



void CAuthenticationManager::ExecuteAuthenticationNow()
{
	//ATLTRACE("%s NOW \n",__FUNCTION__);

	if (! SetEvent(m_hAuthenticationEvent) ) 
	{
		ATLTRACE("SetEvent failed (%d)\n", GetLastError());

		return;
	}
}

CString CAuthenticationManager::GetCookie() 
{
	return m_strCookie;
}


CString CAuthenticationManager::GetUser()
{
	return m_user;
}

BOOL CAuthenticationManager::IsAuthenticated()
{
	return ( m_authResult == CInternetAuthentication::AUTH_OK );
}
