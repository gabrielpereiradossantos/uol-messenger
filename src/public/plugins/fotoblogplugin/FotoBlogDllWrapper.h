#pragma once

typedef void  (*MYSHOWFUNC)(HWND);			// pointer to: int  function(int) 
typedef void  (*MYFINALIZEFUNC)(VOID);		// 
typedef HWND  (*MYGETHWNDFUNC)();			// pointer to: HWND function() 

class CPhotoBlogDllWrapper
{
public:
	CPhotoBlogDllWrapper(void);
	~CPhotoBlogDllWrapper(void);
	BOOL LoadDll(CString dllfilepath);
	BOOL ReleaseDll();
	void ShowFotoBlog(HWND parent);
	void ShowBlog(HWND parent);
protected:
	void SetBlogWindowPosition(HWND hBlogWnd);
	void ClientToScreen(HWND hWnd, CRect& convertingRect);
private:
	BOOL m_initialized;
	MYSHOWFUNC m_pShowFotoBlog;
	MYSHOWFUNC m_pShowBlog;
	MYFINALIZEFUNC m_pFinalizeBlog;
	MYGETHWNDFUNC m_pGetFotoBlogHWND;
	MYGETHWNDFUNC m_pGetBlogHWND;
	HMODULE m_hMod;
	CString m_strDllPath;
};

extern CPhotoBlogDllWrapper g_fotoblog_dll;

