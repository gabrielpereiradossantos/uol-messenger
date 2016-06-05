// uolcrypto.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

CAppModule	_Module;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_Module.Init(NULL, reinterpret_cast<HINSTANCE>(hModule));		
		break;

	case DLL_PROCESS_DETACH:
		_Module.Term();
	}

    return TRUE;
}

