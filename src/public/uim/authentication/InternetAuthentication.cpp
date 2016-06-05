#include "stdafx.h"

#include "InternetAuthentication.h"
#include <memory>
#include <InternetHandle.h>
#include <HttpParamCodec.h>


//////////////////////////////////////////////////////////////////////////
// CInternetAuthentication
//////////////////////////////////////////////////////////////////////////

// Colocar esses defines no recurso
#define _IDS_EMAIL_SUFIX				 "@uol.com.br";
#define _IDS_COOKIE_DOMAIN				 "uol.com.br"
#define _IDS_AUTH_SERVER				 "https://acesso.uol.com.br/login.html"
#define _IDS_AUTH_SERVER_POST_DATA       "dest=REDIR|http://messenger.multi.uol.com.br&user=%s&pass=%s"
#define _IDS_AUTH_COOKIE_NAME			 "CAUBR01="
#define _IDS_GUEST_AUTH_SERVER			 "https://visitante.acesso.uol.com.br/login.html"
#define _IDS_GUEST_AUTH_SERVER_POST_DATA "dest=REDIR|http://messenger.multi.uol.com.br&user=%s&pass=%s"
#define _IDS_GUEST_AUTH_COOKIE_NAME		 "CAUBR01="
#define _IDS_MEMBER_EMAIL_SUFIX			 "@uol.com.br"


CInternetAuthentication::CInternetAuthentication( IAuthenticationFlag& authenticationFlag, 
												 IShutdownFlag& shutdownFlag) : 
	m_authenticationFlag(authenticationFlag), 
	m_shutdownFlag(shutdownFlag)
{
}


CInternetAuthentication::~CInternetAuthentication(void)
{
	ATLTRACE("CInternetAuthentication::~CInternetAuthentication()\n");
}


CInternetAuthentication::AUTH_STATE CInternetAuthentication::DoAuthentication(CString userLogin,
																			  CString userPass,
																			  CString & cookie)
{
	ATLTRACE("%s\n",__FUNCTION__);

	CString strUser = userLogin;

	CString strPass = userPass;

	CString strURL;

	CString strPostData;

	CString strCookieName;		

	CString strOptional;

	USER_TYPE userType;

	if (  (strPass.IsEmpty())  || (userLogin.IsEmpty()) )
	{
		return AUTH_PASS_LOGIN_ERROR;
	}

	int posChar = userLogin.Find('@');

	CString emailSufix = userLogin.Mid(posChar);

	if (emailSufix.CompareNoCase(_IDS_MEMBER_EMAIL_SUFIX)==0) 
	{
		userType = USER_TYPE::MEMBER;
	} else 
	{
		userType = USER_TYPE::GUEST;
	}


	if (USER_TYPE::MEMBER == userType)
	{
		CString strRealm;
		strURL = _IDS_AUTH_SERVER;
		strPostData = _IDS_AUTH_SERVER_POST_DATA;
		strCookieName = _IDS_AUTH_COOKIE_NAME;
		//strRealm.LoadString(IDS_EMAIL_SUFIX);
		//strUser += strRealm;
		//strURL.LoadString(IDS_AUTH_SERVER);
		//strPostData.LoadString(IDS_AUTH_SERVER_POST_DATA);
		//strCookieName.LoadString(IDS_AUTH_COOKIE_NAME);
	}
	else if (USER_TYPE::GUEST == userType)
	{
		strURL = _IDS_GUEST_AUTH_SERVER;
		strPostData = _IDS_GUEST_AUTH_SERVER_POST_DATA;
		strCookieName = _IDS_GUEST_AUTH_COOKIE_NAME;
		//strURL.LoadString(IDS_GUEST_AUTH_SERVER);
		//strPostData.LoadString(IDS_GUEST_AUTH_SERVER_POST_DATA);
		//strCookieName.LoadString(IDS_GUEST_AUTH_COOKIE_NAME);
	}
	else
	{
		return AUTH_PASS_LOGIN_ERROR;
	}

	strOptional.Format(strPostData, CHttpParamCodec::URLEncodeParam(strUser), CHttpParamCodec::URLEncodeParam(strPass));

	CString strCookie;

	cookie  = "";

	AUTH_STATE retState = Authenticate(strURL, strOptional, strCookieName, TRUE, HTTP_STATUS_REDIRECT, NULL, NULL, TRUE,strCookie);

	 cookie = strCookie;

//	 m_authenticationFlag.SetAuthenticated(retState, strCookie);

	 return retState;
}



CInternetAuthentication::AUTH_STATE CInternetAuthentication::Authenticate(const CString& strURL,
										   const CString& strOptional,
										   const CString& strCookieName,					  
										   BOOL  bSecure,
										   DWORD dwHTTPExpectedStatus,
										   LPTSTR lpszLocation,
										   LPDWORD dwLocationSize,
										   BOOL  bRaiseEvent,
										   CString &retCookie
										   )
{	
	CString strUserAgent = GetIEUserAgent();
	ATLASSERT(strUserAgent.IsEmpty() == FALSE);

	// Check shutdown flag
	if (m_shutdownFlag.KillReceived())
	{
		return AUTH_FALSE;
	}

	// Verifica a conexão com a internet
	CInternetHandle hInet = ::InternetOpen(strUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInet)
	{
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - !hInet\n"));
		return AUTH_CONN_ERROR;; 
	}

	CString strCookieDomain;
	//strCookieDomain.LoadString(IDS_COOKIE_DOMAIN);		
	strCookieDomain = _IDS_COOKIE_DOMAIN;
	CString strHeaders = "Content-Type: application/x-www-form-urlencoded\r\n";

	const TCHAR* pszAccept[] = {"text/html", NULL};

	CUrl url;

	if (FALSE == url.CrackUrl(strURL))
	{
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - FALSE == url.CrackUrl(strURL)\n"));
		return AUTH_CONN_ERROR;
	}		

	DWORD_PTR pContext = NULL;

	CInternetHandle hConnect = ::InternetConnect(hInet, 
		url.GetHostName(), 
		url.GetPortNumber(), 
		NULL, 
		NULL, 
		INTERNET_SERVICE_HTTP, 
		0, 
		pContext);

	if (!hConnect)
	{
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - !hConnect\n"));
		return AUTH_CONN_ERROR;
	}

	DWORD dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_AUTO_REDIRECT;

	if (bSecure)
	{
		dwFlags |= INTERNET_FLAG_SECURE;
	}


	CInternetHandle hRequest = ::HttpOpenRequest(hConnect,
		"POST", 
		url.GetUrlPath(), 
		NULL, 
		NULL,
		pszAccept,
		dwFlags,

		pContext);

	if (!hRequest)
	{
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - !hRequest\n"));
		return AUTH_CONN_ERROR;
	}

	// Check shutdown flag
	if (m_shutdownFlag.KillReceived())
	{
		return AUTH_FALSE;
	}

	if (FALSE == ::HttpSendRequest(hRequest, strHeaders, strHeaders.GetLength(), (LPVOID)(LPCTSTR)strOptional, strOptional.GetLength()))
	{			
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - !HttpSendRequest\n"));
		return AUTH_CONN_ERROR;
	}

	TCHAR szStatusCode[20] = {0};
	DWORD dwStatusCodeSize = sizeof(szStatusCode) / sizeof(TCHAR);		

	if (FALSE == ::HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwStatusCodeSize, NULL))
	{
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - !HttpQueryInfo\n"));
		return AUTH_CONN_ERROR;
	}

	DWORD dwStatusCode = (DWORD) _ttol(szStatusCode);

	// Se HTTP_STATUS_REDIRECT == dwStatusCode, autenticaçao OK.
	if (dwHTTPExpectedStatus != dwStatusCode)
	{
		return AUTH_PASS_LOGIN_ERROR;
	}

	if (lpszLocation)
	{
		::HttpQueryInfo(hRequest, HTTP_QUERY_LOCATION, lpszLocation, dwLocationSize, NULL);
	}

	// Check shutdown flag
	if (m_shutdownFlag.KillReceived())
	{
		return AUTH_FALSE;
	}

	DWORD dwCookiesSize = 0;
	::InternetGetCookie(strURL, NULL, NULL, &dwCookiesSize);

	std::auto_ptr<TCHAR> pCokies(new TCHAR[dwCookiesSize + 1]);
	::ZeroMemory(pCokies.get(), (dwCookiesSize + 1) * sizeof(TCHAR));

	if (FALSE == ::InternetGetCookie(strURL, NULL, pCokies.get(), &dwCookiesSize))
	{
		ATLTRACE(_T("CInternetAuthentication::Authenticate() - InternetGetCookie\n"));
		return AUTH_CONN_ERROR;
	}

	CAtlString strServerCookie = (LPCTSTR)pCokies.get();	

	BOOL bAuthenticated = FALSE;

	// Look for authentication cookie
	int nPos = 0;
	CString strCookie;
	CString strResToken = strServerCookie.Tokenize(_T(";"), nPos);

	while (strResToken != "")
	{
		// Check shutdown flag
		if (m_shutdownFlag.KillReceived())
		{
			return AUTH_FALSE;
		}

		if ((-1 != strResToken.Find(strCookieName)) && 
			(-1 == strResToken.Find(strCookieName + "null")))
		{
			// Save cookie
			bAuthenticated = TRUE;

			strCookie += strResToken;
			strCookie += _T(";domain=");
			strCookie += strCookieDomain;

			break;
		}

		strResToken = strServerCookie.Tokenize(_T(";"), nPos);
	}

	if (bAuthenticated)
		retCookie = strCookie;
	//if (bRaiseEvent)
	//{
		//if (bAuthenticated)
		//	m_authenticationFlag.SetAuthenticated(AUTH_OK, strCookie);
	//}

	//Sleep(1000*60);

	return AUTH_OK;
}	

//////////////////////////////////////////////////////////////////////////
// CAuthentication
//////////////////////////////////////////////////////////////////////////

CAuthentication::CAuthentication():m_bIsAuthenticated(FALSE), m_strAuthCookie(_T("")) 
{
//	m_ptrNotify = NULL;
}

CAuthentication::~CAuthentication() 
{
}

CInternetAuthentication::AUTH_STATE CAuthentication::DoAuthentication(CString user, CString pass, CString &cookie)
{
	CInternetAuthentication *ptr;
	
	ptr = new CInternetAuthentication(*this, *this);

	CInternetAuthentication::AUTH_STATE ret = ptr->DoAuthentication(user,pass,cookie);

	delete ptr;

	return ret;

}


LONG CAuthentication::IsAuthenticated(int nUserType ) const
{
	if (-1 == nUserType)
	{
		return m_bIsAuthenticated;
	}

	return ((m_nAuthUserType == nUserType) && m_bIsAuthenticated);
}

void CAuthentication::SetAuthenticated(LONG b, LPCTSTR szAuthCookie)
{	
	//::InterlockedExchange(&m_bIsAuthenticated, b);	

	//PostBroadcast(OnAuthenticated);

	if (szAuthCookie)
	{
		m_strAuthCookie = szAuthCookie;
	}
	else
	{
		m_strAuthCookie.Empty();
	}

	//if (m_ptrNotify!=NULL) 
	//	m_ptrNotify->OnAuthenticate(  b, m_strAuthCookie);

	//__raise OnAuthenticated(b, m_nAuthUserType);		
}

LONG CAuthentication::KillReceived() const
{
	return 0L;
}

