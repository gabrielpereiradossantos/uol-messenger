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
 

// MinDependencySpecialFolder.cpp: implementation of the CMinDependencySpecialFolder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MinDependencySpecialFolder.h"


const CString CMinDependencySpecialFolder::SHELL_WINDOWS_SETTINGS = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
const CString CMinDependencySpecialFolder::SHELL_USER_SETTINGS = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");
const CString CMinDependencySpecialFolder::SHELL_ALL_USERS_SETTINGS = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMinDependencySpecialFolder::CMinDependencySpecialFolder()
{

}

CMinDependencySpecialFolder::~CMinDependencySpecialFolder()
{

}


CString CMinDependencySpecialFolder::GetProgramFiles()
{
	return GetFolder(HKEY_LOCAL_MACHINE, SHELL_WINDOWS_SETTINGS, "ProgramFilesDir");
}

CString CMinDependencySpecialFolder::GetCommonPrograms()
{
	return GetFolder(HKEY_LOCAL_MACHINE, SHELL_ALL_USERS_SETTINGS, "Common Programs");	
}

CString CMinDependencySpecialFolder::GetAppData()
{
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "AppData");	
}

CString CMinDependencySpecialFolder::GetCommonAppData()
{
	return GetFolder(HKEY_LOCAL_MACHINE, SHELL_ALL_USERS_SETTINGS, "Common AppData");	
}

CString CMinDependencySpecialFolder::GetPrograms()
{
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "Programs");	
}

CString CMinDependencySpecialFolder::GetDesktopDirectory()
{
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "Desktop");
}

CString CMinDependencySpecialFolder::GetCommonDesktopDirectory()
{
	return GetFolder(HKEY_LOCAL_MACHINE, SHELL_ALL_USERS_SETTINGS, "Common Desktop");
}

CString CMinDependencySpecialFolder::GetCommonStartmenu()
{
	return GetFolder(HKEY_LOCAL_MACHINE, SHELL_ALL_USERS_SETTINGS, "Common Start Menu");
}

CString CMinDependencySpecialFolder::GetStartMenu()
{
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "Start Menu");
}

CString CMinDependencySpecialFolder::GetHistory()
{
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "History");
}

CString CMinDependencySpecialFolder::GetPersonal()
{
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "Personal");
}

CString CMinDependencySpecialFolder::GetTemp()
{
	CString str;

	BOOL bOK = ::GetTempPath(MAX_PATH, str.GetBuffer(MAX_PATH + 1));

	str.ReleaseBuffer();

	if (bOK)
	{
		CString strFullPath;
		GetLongPathName(str, strFullPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);

		return strFullPath;
	}

	throw new CSpecialFolderException;
}

CString CMinDependencySpecialFolder::GetWindows()
{
	CString str;

	BOOL bOK = ::GetWindowsDirectory(str.GetBuffer(MAX_PATH + 1), MAX_PATH);

	str.ReleaseBuffer();

	if (bOK)
	{
		PatchDir(str);

		return str;
	}

	throw new CSpecialFolderException;
}

CString CMinDependencySpecialFolder::GetWindowsSystem()
{
	CString str;

	BOOL bOK = ::GetSystemDirectory(str.GetBuffer(MAX_PATH + 1), MAX_PATH);

	str.ReleaseBuffer();

	if (bOK)
	{
		PatchDir(str);

		return str;
	}

	throw new CSpecialFolderException;
}

CString CMinDependencySpecialFolder::GetCommonStartUpFolder()
{
	return GetFolder(HKEY_LOCAL_MACHINE, SHELL_ALL_USERS_SETTINGS, "Startup");
}

CString CMinDependencySpecialFolder::GetStartUpFolder()
{ 
	return GetFolder(HKEY_CURRENT_USER, SHELL_USER_SETTINGS, "Startup");
}

CString CMinDependencySpecialFolder::GetFolder(HKEY hRootKey, const CString& strKey, const CString& strValue)
{
	CRegKey reg;

	if (ERROR_SUCCESS == reg.Open(hRootKey, strKey, KEY_READ))
	{
		CString str;
		ULONG lSize = MAX_PATH;
		ULONG lRet =  reg.QueryStringValue(strValue, str.GetBuffer(lSize + 1), &lSize);

		str.ReleaseBuffer();
		
		if (lRet == ERROR_SUCCESS)
		{
			PatchDir(str);

			return str;
		}
	}
	
	throw new CSpecialFolderException;
}

void CMinDependencySpecialFolder::PatchDir(CString &strPath)
{
	if(strPath.Right(1) != "\\")
	{
		strPath+="\\";
	}	
}