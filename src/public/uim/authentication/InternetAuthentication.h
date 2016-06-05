#pragma once

//#include "LoginDlg.h"
//#include "UserOptions.h"
//#include "resource.h"

#include "../controller/utils.h"

//////////////////////////////////////////////////////////////////////////
// CAuthenticationNotify
//////////////////////////////////////////////////////////////////////////

class CAuthenticationNotify {
public:
	virtual void OnAuthenticate(LONG bAuthenticated, CString cookie)=0;
};
MAKEAUTOPTR(CAuthenticationNotify);

//////////////////////////////////////////////////////////////////////////
// CInternetAuthentication
//////////////////////////////////////////////////////////////////////////

class CInternetAuthentication //: public IRunnableJob
{
	static const enum USER_TYPE { MEMBER = 0, GUEST = 1, ANONYMOUS = 2 };
public:
	static const enum AUTH_STATE { AUTH_OK = 0, AUTH_PASS_LOGIN_ERROR , AUTH_CONN_ERROR , AUTH_FALSE};
public:
	CInternetAuthentication(IAuthenticationFlag& authenticationFlag, IShutdownFlag& shutdownFlag);
	virtual ~CInternetAuthentication(void);
public:
	AUTH_STATE  DoAuthentication(CString userLogin, CString userPass, CString &cookie);
	AUTH_STATE  Authenticate(const CString& strURL,
					  const CString& strOptional,
					  const CString& strCookieName,					  
					  BOOL  bSecure,
					  DWORD dwHTTPExpectedStatus,
					  LPTSTR lpszLocation,
					  LPDWORD dwLocationSize,
					  BOOL  bRaiseEvent,
					  CString &retCookie
					  );
private:
	CString m_userLogin;
	CString m_userPass;
	int m_userType;
	IAuthenticationFlag&		m_authenticationFlag;
	IShutdownFlag&				m_shutdownFlag;
};

//////////////////////////////////////////////////////////////////////////
// CAuthentication
//////////////////////////////////////////////////////////////////////////

class CAuthentication : 
	public IAuthenticationFlag ,
	public IShutdownFlag
{
public:
	CAuthentication();
	~CAuthentication();
	CInternetAuthentication::AUTH_STATE DoAuthentication(CString user, CString pass, CString & cookie );
private:
	CString		m_strAuthCookie;
	LONG		m_bIsAuthenticated;
	int			m_nAuthUserType;
	//CAuthenticationNotify *m_ptrNotify;
	LONG IsAuthenticated(int nUserType = -1) const;
	void SetAuthenticated(LONG b, LPCTSTR szAuthCookie);
	LONG KillReceived() const;
};
