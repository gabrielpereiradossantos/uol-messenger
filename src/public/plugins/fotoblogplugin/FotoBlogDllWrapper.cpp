#include "StdAfx.h"
#include ".\fotoblogdllwrapper.h"

CPhotoBlogDllWrapper g_fotoblog_dll;

extern IUOLMessengerFacade * g_pUOLMessengerFacade;
CPhotoBlogDllWrapper::CPhotoBlogDllWrapper(void)
{
	m_initialized  = FALSE;
	m_pShowFotoBlog = NULL;
	m_pShowBlog = NULL;
	m_hMod = NULL;
}

CPhotoBlogDllWrapper::~CPhotoBlogDllWrapper(void)
{
	if (m_initialized) {
		ReleaseDll();
		m_initialized  = FALSE;
		m_pShowFotoBlog = NULL;
		m_pShowBlog = NULL;
		m_hMod = NULL;
	}
}


BOOL CPhotoBlogDllWrapper::LoadDll(CString dllfilepath)
{
	if (m_initialized==TRUE) 
		return FALSE;

	m_initialized = FALSE;

	m_strDllPath = dllfilepath;

	m_hMod = LoadLibrary(m_strDllPath);
	if (m_hMod==NULL) {
		return FALSE;
	}

	m_pShowFotoBlog = (MYSHOWFUNC) GetProcAddress(m_hMod, "ShowPhotoBlogDlg");
	if (m_pShowFotoBlog==NULL)
		return FALSE;

	m_pShowBlog = (MYSHOWFUNC) GetProcAddress(m_hMod, "ShowBlogDlg");
	if (m_pShowBlog==NULL)
		return FALSE;

	m_pFinalizeBlog = (MYFINALIZEFUNC) GetProcAddress(m_hMod, "Finalize");
	if (m_pFinalizeBlog==NULL)
		return FALSE;

	m_pGetFotoBlogHWND = (MYGETHWNDFUNC) GetProcAddress(m_hMod, "GetPhotoBlogHWND");
	if (m_pGetFotoBlogHWND==NULL)
		return FALSE;

	m_pGetBlogHWND = (MYGETHWNDFUNC) GetProcAddress(m_hMod, "GetBlogHWND");
	if (m_pGetBlogHWND==NULL)
		return FALSE;

	m_initialized = TRUE;
	return TRUE;
	
}

BOOL CPhotoBlogDllWrapper::ReleaseDll()
{
	if (m_initialized==TRUE)
	{
		m_pFinalizeBlog();
		BOOL  bRes;
		m_initialized = FALSE;
		bRes = FreeLibrary(m_hMod); 
		return (bRes!=0);
	}
	return FALSE;
}

void CPhotoBlogDllWrapper::ShowFotoBlog(HWND parent)
{
	if (m_initialized==TRUE)
	{
		//g_pUOLMessengerFacade->GetAuthenticationManager()->ExecuteAuthenticationNow();
		g_pUOLMessengerFacade->GetAuthenticationManager()->CallAuthentication();
		m_pShowFotoBlog(parent);
		
		SetBlogWindowPosition(m_pGetFotoBlogHWND());
	}
}

void CPhotoBlogDllWrapper::ShowBlog(HWND parent)
{
	if (m_initialized==TRUE)
	{
		//g_pUOLMessengerFacade->GetAuthenticationManager()->ExecuteAuthenticationNow();
		g_pUOLMessengerFacade->GetAuthenticationManager()->CallAuthentication();
		m_pShowBlog(parent);
		
		SetBlogWindowPosition(m_pGetBlogHWND());
	}
}

void CPhotoBlogDllWrapper::SetBlogWindowPosition(HWND hBlogWnd)
{
	IUOLMessengerUIManagerPtr pUIManager = g_pUOLMessengerFacade->GetUIManager();
	
	if ( (pUIManager) && (hBlogWnd != NULL) && (::IsWindow(hBlogWnd)) )
	{
		CRect blogWindowRect;
		CRect rectDesktop;
		CRect appRect;
		int xCoord, yCoord;
		
		pUIManager->GetAppWindow().GetClientRect(&appRect);
		pUIManager->GetAppWindow().ClientToScreen(&appRect);
		
		::GetWindowRect(hBlogWnd, &blogWindowRect);
		ClientToScreen(::GetDesktopWindow(), blogWindowRect);
		
		::GetWindowRect(::GetDesktopWindow(), &rectDesktop);
		
		if (appRect.left > blogWindowRect.Width())
		{
			xCoord = appRect.left - blogWindowRect.Width();
		} 
		else
		{
			xCoord = appRect.right;
		}
		
		if (appRect.bottom > rectDesktop.bottom)
		{
			yCoord = appRect.top;
		} 
		else
		{
			yCoord = appRect.bottom - blogWindowRect.Height();
		}
		
		::SetWindowPos(hBlogWnd, NULL, xCoord, yCoord, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
}

void CPhotoBlogDllWrapper::ClientToScreen(HWND hWnd, CRect& convertingRect)
{
	CPoint topLeft, bottomRight;

	topLeft = convertingRect.TopLeft();
	bottomRight = convertingRect.BottomRight();
	
	::ClientToScreen(hWnd, &topLeft);
	::ClientToScreen(hWnd, &bottomRight);
	
	convertingRect.SetRect(topLeft, bottomRight);
}
