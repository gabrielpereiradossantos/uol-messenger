#pragma once

#include <atlstr.h>
#include <atlcoll.h>
#include <WinInet.h>

#include <set>


enum AuthenticationResult { AUTH_OK, AUTH_TIMEOUT };

//#include "authentication/InternetAuthentication.h"

#include "../Authentication/InternetAuthentication.h" 
#include <interfaces/IUOLMessengerAuthenticationManager.h>

//////////////////////////////////////////////////////////////////////////
// CAuthenticationObserver
//////////////////////////////////////////////////////////////////////////

//class CAuthenticationObserver 
//{
//public:
//
//	virtual void OnAuthenticate(const CString & cookie, AuthenticationResult status) = 0;
//};

//////////////////////////////////////////////////////////////////////////

//class __declspec(novtable) IUOLMessengerAuthenticationManager
//{
//public:
//	//virtual ~IUOLMessengerAuthenticationManager() {}
//	//virtual BOOL ValidAuthentication(CString user, CString pass) = 0;
//	//virtual BOOL ReAuthentication() = 0;
//	//virtual void StartReAuthencitaion()=0;
//	//virtual void StopReAuthencitaion()=0;
//	//virtual void SetNotify(IUOLMessengerAuthenticationNotify * ptrNotify)=0;
//	virtual BOOL DoAuthentication(const CString userlogin, const CString password, BOOL reauthenticate = FALSE) = 0;
//	virtual BOOL IsAuthenticated()=0;
//	virtual CString GetUser()=0;
//	virtual CString GetCookie()=0;
//};

class CAuthenticationManager : public IUOLMessengerAuthenticationManager
{
public:

	CAuthenticationManager();

	virtual ~CAuthenticationManager();

	void StopReAuthentication();

	BOOL DoAuthentication(const CString userlogin, const CString password, BOOL reauthenticate );

	void RegisterObserver(CAuthenticationObserver * pObserver);

	void UnregisterObserver(CAuthenticationObserver * pObserver);

	void NotifyAll(	void (CAuthenticationObserver::*pfnHandler)(const CString & cookie, LONG status), 
		const CString cookie,
		LONG status);

	CString GetCookie() ;

	virtual CString GetUser();

	virtual BOOL IsAuthenticated();

	virtual void ExecuteAuthenticationNow();

	void CallAuthentication();

private:
	

	BOOL InitAuthenticationThread();

	void MyAuthenticationThreadFunction();

	static UINT WINAPI MyAuthenticationThread(LPVOID pParam);

	static UINT WINAPI StartAuthenticationThread(LPVOID pParam);

	HANDLE m_hAuthenticationEvent; 
	HANDLE m_hThreadAuth; 
	HANDLE m_hMyThread;
	unsigned int m_hMyThreadID;
	unsigned int m_hMyThreaAuthdID;
	CComAutoCriticalSection		m_criticalSection;
	CAtlList<CAuthenticationObserver*> m_listObservers;
	BOOL m_bReAuthenticate;
	CString m_strCookie;
	LONG m_authResult;
	CString m_user;
	CString m_password;
};





//#pragma once
// 
//#include <atlstr.h>
//
//#include "../Authentication/InternetAuthentication.h" 
//#include <interfaces/IUOLMessengerAuthenticationManager.h>
//
//class CAuthenticationManager :
//	public CAuthenticationNotify ,
//	public IUOLMessengerAuthenticationManager
//{
//public:
//	//enum AuthenticationStatus { AUTH_PASS_LOGIN_ERRO, AUTH_CONNECTION_ERRO, AUTH_OK };
//	CAuthenticationManager(BOOL requirePassword = FALSE);
//	~CAuthenticationManager(void);
//
//	static CString Encode(CString filename);
//	static bool isOrdinaryChar(char c);
//
//	//IUOLMessengerAuthenticationManager
//	virtual BOOL DoAuthentication(CString user, CString pass);
//
//	virtual BOOL ReAuthentication();
//
//	virtual BOOL ValidAuthentication(CString user, CString pass);
//
//	virtual BOOL IsAuthenticated();
//
//	virtual void StartReAuthencitaion();
//
//	virtual void StopReAuthencitaion();
//
//	virtual CString GetCookie();
//
//	virtual CString GetUser();
//
//	virtual void SetNotify(IUOLMessengerAuthenticationNotify * ptrNotify);
//
//	//CAuthenticationNotify
//	virtual void OnAuthenticate(LONG Authentication, CString cookie);
//private:
//	LONG TryAuthenticate();					// Call CInternetAuthenticate	
//
//	BOOL StartReAuthenticateThread();		// ReAuthenticateThread
//	BOOL StartThread();						// Call StartAuthThread
//
//	static UINT WINAPI AuthenticateThread(LPVOID pParam);	// Call TryAuthenticate	
//	static UINT WINAPI ReAuthenticateThread(LPVOID pParam);	// Loop for reAuthenticate
//	static UINT WINAPI StartAuthThread(LPVOID pParam);		// Call AuthenticateThread
//															
//	HANDLE m_hThread;				
//	HANDLE m_hThreadReAuth;
//
//	CString m_cookie;
//	CString m_user;
//	CString m_pass;
//	
//	BOOL m_bReAuthenticate;
//	BOOL m_bIsAuthenticated;
//	BOOL m_bRequirePassword;
//	BOOL m_bStopReathentication;
//
//	LONG m_lastAuthReturn;
//
//	IUOLMessengerAuthenticationNotify *  m_pNotify;
//};
//
//
