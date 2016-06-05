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
 
// IdleTracker.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "IdleTracker.h"


static HINSTANCE g_hInstance = NULL;

static HANDLE g_hMapObject = NULL;
static LPDWORD g_pdwLastTime = NULL;

static HHOOK g_hKeyboardHook = NULL;
static HHOOK g_hMouseHook = NULL;
static POINT g_ptMouse;


// From winuser.h
/*
typedef struct tagLASTINPUTINFO {
    UINT cbSize;
    DWORD dwTime;
} LASTINPUTINFO, * PLASTINPUTINFO;
*/

typedef BOOL (WINAPI *PFN_GETLASTINPUTINFO)(LASTINPUTINFO *);
static HMODULE g_hUser32 = NULL;

static PFN_GETLASTINPUTINFO g_pfnGetLastInputInfo = NULL;


static LPDWORD SetupSharedMemory() {
	BOOL fInit;

	/// Set up the shared memory
	g_hMapObject = ::CreateFileMapping((HANDLE) 0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(DWORD),
		"idletracker");

	if (g_hMapObject == NULL)
	{
		return NULL;
	}

	// The first process to attach initializes memory
	fInit = (GetLastError() != ERROR_ALREADY_EXISTS);

	// Get a pointer to the file-mapped shared memory
	g_pdwLastTime = (DWORD*) MapViewOfFile(g_hMapObject,
		FILE_MAP_WRITE,
		0,
		0,
		0);

	if (g_pdwLastTime == NULL)
	{
		return NULL;
	}

	*g_pdwLastTime = GetTickCount();

	return g_pdwLastTime;
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!(nCode < 0))
	{
		if (g_pdwLastTime == NULL)
		{
			g_pdwLastTime = SetupSharedMemory();
		}

		if (g_pdwLastTime)
		{
			*g_pdwLastTime = GetTickCount();
		}
	}

	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}


LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// We need to verify that the Mouse pointer has actually moved
	if (!(nCode < 0) && 
		!((g_ptMouse.x == ((MOUSEHOOKSTRUCT*)lParam)->pt.x) &&
		  (g_ptMouse.y == ((MOUSEHOOKSTRUCT*)lParam)->pt.y)))
	{
		g_ptMouse.x = ((MOUSEHOOKSTRUCT*)lParam)->pt.x;
		g_ptMouse.y = ((MOUSEHOOKSTRUCT*)lParam)->pt.y;

		if (g_pdwLastTime == NULL)
		{
			g_pdwLastTime = SetupSharedMemory();
		}

		if (g_pdwLastTime)
		{
			*g_pdwLastTime = GetTickCount();
		}
	}
	
	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}



IDLETRACKER_API DWORD GetLastActivityTickCount()
{
	DWORD dwResult = 0;

	// If we have GetLastInputInfo then use it, otherwise use the hooks
	if (g_pfnGetLastInputInfo != NULL)
	{
		LASTINPUTINFO lii = {0};
		lii.cbSize = sizeof(lii);

		if (g_pfnGetLastInputInfo(&lii))
		{
			dwResult = lii.dwTime;
		}
	} 
	else
	{
		if (g_pdwLastTime == NULL)
		{
			g_pdwLastTime = SetupSharedMemory();
		}

		if (g_pdwLastTime)
		{
			dwResult = *g_pdwLastTime;
		}
	}

	return dwResult;
}


IDLETRACKER_API BOOL IdleTrackInit()
{
	// Is GetLastInputInfo available?
	g_hUser32 = ::LoadLibrary("user32.dll");
	if (g_hUser32)
	{
		g_pfnGetLastInputInfo = (PFN_GETLASTINPUTINFO) ::GetProcAddress(g_hUser32, "GetLastInputInfo");
	}

	// If we couldn't find GetLastInputInfo then fall back onto the hooking scheme
	if (g_pfnGetLastInputInfo == NULL)
	{
		// Set up the shared memory
		g_pdwLastTime = SetupSharedMemory();
		if (g_pdwLastTime == NULL)
		{
			return FALSE;
		}

		*g_pdwLastTime = GetTickCount();

		// Set up the keyboard hook
		g_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hInstance, 0);

		if (g_hKeyboardHook == NULL)
		{
			::UnmapViewOfFile(g_pdwLastTime);
			::CloseHandle(g_hMapObject);
			return FALSE;
		}

		// Set up the mouse hook
		g_hMouseHook = ::SetWindowsHookEx(WH_MOUSE, MouseProc, g_hInstance, 0);
		if (g_hMouseHook == NULL)
		{
			::UnhookWindowsHookEx(g_hKeyboardHook);
			::UnmapViewOfFile(g_pdwLastTime);
			::CloseHandle(g_hMapObject);
			return FALSE;
		}
	}

	return TRUE;
}


IDLETRACKER_API BOOL IdleTrackFinalize()
{
	if (g_hUser32 != NULL)
	{
		::FreeLibrary(g_hUser32);
	}

	if (g_hKeyboardHook)
	{
		::UnhookWindowsHookEx(g_hKeyboardHook);
	}

	if (g_hMouseHook)
	{
		::UnhookWindowsHookEx(g_hMouseHook);
	}

	if (g_pdwLastTime)
	{
		::UnmapViewOfFile(g_pdwLastTime);
	}

	if (g_hMapObject)
	{
		::CloseHandle(g_hMapObject);
	}

	return TRUE;
}



BOOL APIENTRY DllMain(HINSTANCE hModule, 
                      DWORD  dwReason, 
                      LPVOID /*lpReserved*/)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hModule;
		g_ptMouse.x = 0;
		g_ptMouse.y = 0;
		break;
	//case DLL_THREAD_ATTACH:
	//case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}



/*
// This is an example of an exported variable
IDLETRACKER_API int nIdleTracker=0;

// This is an example of an exported function.
IDLETRACKER_API int fnIdleTracker()
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see IdleTracker.h for the class definition
CIdleTracker::CIdleTracker()
{ 
	return; 
}
*/