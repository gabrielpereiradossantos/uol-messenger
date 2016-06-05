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


class CWindowEx : public CWindow
{
public:
	CWindowEx(HWND  hWnd = NULL):
			CWindow(hWnd)
	{
	}

private:	
	typedef struct tag_FIND_ID
	{
		LONG    lId;	
		HWND    hMatchWnd;
	} FIND_ID, * LPFIND_ID;

	static BOOL CALLBACK ChildWindowIDEnumProc(HWND hWnd, LPARAM lParam)
	{
		LPFIND_ID pFind = (LPFIND_ID)lParam;
		BOOL bRet = TRUE;
	    
		LONG lCurr = ::GetWindowLong(hWnd, GWL_ID);

		if (lCurr == pFind->lId)
		{
			pFind->hMatchWnd = hWnd;
			bRet = FALSE;
		}

		return (bRet);
	}

	static BOOL CALLBACK EnumChildWindowProc(HWND hWnd, LPARAM lParam)
	{		
		CAtlArray<HWND>* pArray = reinterpret_cast<CAtlArray<HWND>*>(lParam);
		
		pArray->Add(hWnd);

		return TRUE;
	}

public:	    
    CString GetCaption()
	{
		const int MAX_CAPTION = 1024;
		CString str;

		TCHAR szBuff [MAX_CAPTION + 1] = {0};		

		if (::SendMessage(m_hWnd, WM_GETTEXT, MAX_CAPTION, reinterpret_cast<LPARAM>(szBuff)))
		{	
			str = szBuff;							
		}

		return str;
	}

    CString GetTitle()
	{
		return GetCaption();
	}

    CString GetClassName()
	{
		CString str;
		
		TCHAR szBuff[MAX_PATH + 1] = {0};		

		if (0 != ::GetClassName(m_hWnd, szBuff, MAX_PATH))
		{
			str = szBuff;				
		}		

		return str;
	}

    LONG GetTop()
	{
		RECT r;		
		GetWindowRect(&r);		

		return r.top;		
	}

    LONG GetBottom()
	{
		RECT r;		
		GetWindowRect(&r);		

		return r.bottom;		
	}

    LONG GetLeft()
	{
		RECT r;		
		GetWindowRect(&r);		

		return r.left;		
	}

    LONG GetRight()
	{
		RECT r;		
		GetWindowRect(&r);		

		return r.right;		
	}

    BOOL GetValid()
	{
		return ::IsWindow(m_hWnd);
	}

    BOOL GetEnabled()
	{
		return  IsWindowEnabled();
	}	

 	DWORD GetProcessId()
	{
		DWORD dwPID = 0;
		DWORD dwTID = ::GetWindowThreadProcessId(m_hWnd , &dwPID); 

        if (0 == dwTID)
        {
            dwPID = 0;
        }

		return dwPID;        
	}

    DWORD GetThreadId()
	{
		return ::GetWindowThreadProcessId(m_hWnd, NULL);
	}

    LONG GetId()
	{
		return ::GetWindowLong(m_hWnd , GWL_ID);        
	}

    BOOL GetVisible()
	{
		return ::IsWindowVisible(m_hWnd); 
	}

    void SetWindowSize(LONG X, LONG Y, LONG Width, LONG Height)
	{
		::MoveWindow(m_hWnd, X, Y, Width, Height, TRUE);
	}

    BOOL SetForegroundWindow(void)
	{
		if (IsWindow())
		{
			DWORD dwWinPID = 0;
			DWORD dwWinTID = ::GetWindowThreadProcessId(m_hWnd , &dwWinPID);

			if (0 != dwWinTID)
			{
				BOOL bRet = AttachThreadInput(GetCurrentThreadId(),
											  dwWinTID,
											  TRUE );
				
				ATLASSERT(FALSE != bRet);

				if (FALSE != bRet)
				{					
					bRet = ::SetForegroundWindow(m_hWnd);
					ATLASSERT(FALSE != bRet);
	                
					BOOL bRet2 = ::AttachThreadInput(GetCurrentThreadId(),
													 dwWinTID,
													 FALSE);
	                                            
					return  ((FALSE != bRet2) && (FALSE != bRet));					
				}
			}
		}

		return FALSE;
	}

	void SetText(const CString& strText)
	{
		SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)strText);
	}

#if(_WIN32_WINNT >= 0x0500)

    BOOL SetFocusWindow(void)
	{
		if (IsWindow())
		{
			DWORD dwWinPID = 0;
			DWORD dwWinTID = ::GetWindowThreadProcessId(m_hWnd , &dwWinPID);

			if (0 != dwWinTID)
			{
				BOOL bRet = ::AllowSetForegroundWindow(dwWinPID);

				if (FALSE != bRet)
				{	
					bRet = ::AttachThreadInput(GetCurrentThreadId(),
											   dwWinTID,
											   TRUE);
					if (FALSE != bRet)
					{
					
						HWND hFocus = ::SetFocus(m_hWnd);
	                    
						bRet = ::AttachThreadInput(GetCurrentThreadId(),
												   dwWinTID,
												   TRUE);

						return  ((FALSE != bRet) && (NULL != hFocus));
					}
				}
			}
		}

		return FALSE;
	}

#endif // (_WIN32_WINNT >= 0x0500)

    HWND FindChildWindow(const CString& strClass, const CString& strTitle)
	{
		if (IsWindow())
		{	
			LPCTSTR szClass = strClass;
			LPCTSTR szTitle = strTitle;

			if (0 == ::_tcslen(szClass))
			{
				szClass = NULL;
			}

			if (0 == ::_tcslen(szTitle))
			{
				szTitle = NULL;
			}

			return ::FindWindowEx(
				m_hWnd, 
				NULL, 
				szClass, 
				szTitle);
		}

		return FALSE;
	}
   
    HWND FindChildWindowByClass(const CString& strClass)
	{
		return FindChildWindow(strClass, "");
	}

    HWND FindChildWindowByTitle(const CString& strTitle)
	{
		return FindChildWindow("", strTitle);
	}

    HWND FindChildWindowWithID(LONG iID)
	{
		 if (IsWindow())
		 {			
			FIND_ID stFind;
			stFind.hMatchWnd = NULL;
			stFind.lId       = iID;

			::EnumChildWindows(m_hWnd ,
							&ChildWindowIDEnumProc ,
							(LPARAM)&stFind);
	                           
			if (NULL != stFind.hMatchWnd)
			{	            
				return stFind.hMatchWnd;            
			}        
		}

		return FALSE;
	}

	BOOL EnumChildren(CAtlArray<HWND>& arrayWindows)
	{
		return GetChildren(arrayWindows);
	}

    BOOL GetChildren(CAtlArray<HWND>& arrayWindows)
	{
		arrayWindows.RemoveAll();

		if (IsWindow())
		{
			return ::EnumChildWindows(m_hWnd,
									  &EnumChildWindowProc,
									  (LPARAM)&arrayWindows);
		}

		return FALSE;
	}

	LRESULT SendMessageTimeout(UINT Msg, UINT uTimeout, WPARAM wParam = 0, LPARAM lParam = 0, 
							   PDWORD_PTR lpdwResult = NULL, UINT fuFlags = SMTO_NORMAL)
	{
		DWORD dwDummy;

		if (NULL == lpdwResult)
		{
			lpdwResult = &dwDummy;
		}

		return ::SendMessageTimeout(m_hWnd, Msg, wParam, lParam, fuFlags, uTimeout, lpdwResult);
	}
};