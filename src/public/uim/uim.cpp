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
 
// uim.cpp : main source file for uim.exe
//

#include "stdafx.h"

#include "resource.h"

#include "controller/UIMApplication.h"

#include "./CrashReportHelper.h"

using namespace Gdiplus;


CAppModule _Module;


HANDLE	g_hSingleInstanceMutex;


BOOL CheckSingleInstance()
{
	CString strMutex;
	strMutex.LoadString(IDS_MUTEX_SINGLE_INSTANCE);

	g_hSingleInstanceMutex = CreateMutex(NULL, TRUE, strMutex);
	
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{		
		g_hSingleInstanceMutex = NULL;
		return FALSE;
	}

	return TRUE;
}


void CloseSingleInstanceMutex()
{
	if (g_hSingleInstanceMutex != NULL)
	{
		::CloseHandle(g_hSingleInstanceMutex);
	}
}


#include <debugstackwalk.h>


int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	int nRet = 0;
	
	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::AllocApplication();
	CUIMApplication* pUIMApplication = static_cast<CUIMApplication*>(pUOLMessengerFacade);

	if (pUIMApplication)
	{
		CCrashReportHelper::GetInstance()->RegisterExceptionFilter();
		
		nRet = pUIMApplication->Init(lpstrCmdLine);

		if (nRet == CUIMApplication::AIE_ERROR_SUCCESS)
		{
			nRet = pUIMApplication->Run();
		}

		pUIMApplication->Finalize();
		
		CCrashReportHelper::GetInstance()->UnregisterExceptionFilter();
		
		pUIMApplication->ReleaseApplication();		
	}	
	
	return nRet;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	::_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | 
					  _CRTDBG_CHECK_ALWAYS_DF |
					  _CRTDBG_LEAK_CHECK_DF);
#endif

	HRESULT hRes = ::CoInitialize(NULL);

	OleInitialize(0);

// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES);	// add flags to support other controls
	AtlAxWinInit();

	//hRes = _Module.Init(NULL, hInstance);
	hRes = _Module.Init(NULL, hInstance, &LIBID_ATLLib);
	ATLASSERT(SUCCEEDED(hRes));

	HMODULE hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
	ATLASSERT(hInstRich != NULL);	

	int nRet = 0;
	if (CheckSingleInstance())
	{
		// Initialize GDI+ library
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		nRet = Run(lpstrCmdLine, nCmdShow);

		// Clean-up GDI+ library
		GdiplusShutdown(gdiplusToken);
	}

	CloseSingleInstanceMutex();

	if (nRet == CUIMApplication::AIE_ERROR_RESTART)
	{
		LPTSTR pszCommandLine = ::GetCommandLine();

		PROCESS_INFORMATION pi;
        STARTUPINFO si;

		::ZeroMemory(&si, sizeof(STARTUPINFO));
        ::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        
		BOOL bRet = ::CreateProcess(NULL,
				pszCommandLine,
				NULL, NULL, FALSE,
				0,
				NULL, NULL,
				&si, &pi);
        ATLASSERT(FALSE != bRet);
	}

	if (hInstRich != NULL)
	{
		::FreeLibrary(hInstRich);
	}

	OleUninitialize();

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
