// UOLFonePlugin.h : main header file for the UOLFonePlugin DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CUOLFonePluginApp
// See UOLFonePlugin.cpp for the implementation of this class
//

class CUOLFonePluginApp : public CWinApp
{
public:
	CUOLFonePluginApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
