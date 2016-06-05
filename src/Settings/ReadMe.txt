========================================================================
    DYNAMIC LINK LIBRARY : Settings Project Overview
========================================================================

AppWizard has created this Settings DLL for you.  
This file contains a summary of what you will find in each of the files that
make up your Settings application.


Settings.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

Settings.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it  
	will not produce a .lib file when it is built. If you wish this project 
	to be a project dependency of some other project, you will either need to 
	add code to export some symbols from the DLL so that an export library 
	will be produced, or you can set the Ignore Input Library property to Yes 
	on the General propert page of the Linker folder in the project's Property 
	Pages dialog box.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Settings.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////



const std::string str((LPCTSTR)strKey);
	CNode* pNode = m_pRoot;

	std::string::const_iterator itBegin = str.begin();
	std::string::const_iterator itMark = itBegin;
	std::string::const_iterator itEnd = str.end();
	
	for (; itMark != itEnd; itMark++)
	{
		if (*itMark == '.')
		{
			CString strSubKey(static_cast<LPCTSTR>(itBegin), static_cast<int>(itMark - itBegin));

			pNode = GetNode(pNode, strSubKey);

			itBegin = itMark + 1;
		}
	}		

	/*CString strSubKey(itBegin, itEnd);
	pNode = GetNode(pNode, strSubKey);	*/