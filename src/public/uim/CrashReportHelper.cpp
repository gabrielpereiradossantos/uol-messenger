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

#include "stdafx.h"
#include "CrashReportHelper.h"

#include "./Utility.h"
#include "./controller/UIMApplication.h"

#include <commands/OpenBugReportCommand.h>


// Unhandled exception callback set with SetUnhandledExceptionFilter().
//
LONG WINAPI CustomUnhandledExceptionFilter(PEXCEPTION_POINTERS pExInfo)
{
	//MessageBox(NULL, "Para aqui pra poder anexar ao processo!", "", MB_OK);
	
	CCrashReportHelper::GetInstance()->ShowCrashReport(pExInfo);
	
	return EXCEPTION_EXECUTE_HANDLER;
}


CCrashReportHelper::CCrashReportHelper()
{
}


CCrashReportHelper::~CCrashReportHelper()
{
}


CCrashReportHelper* CCrashReportHelper::GetInstance()
{
	static CCrashReportHelper crashReportHelper;
	
	return &crashReportHelper;
}


void CCrashReportHelper::RegisterExceptionFilter()
{
	m_pOldFilter = SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
}


void CCrashReportHelper::UnregisterExceptionFilter()
{
	if (m_pOldFilter != NULL)
	{
		SetUnhandledExceptionFilter(m_pOldFilter);
	}
}


void CCrashReportHelper::ShowCrashReport(PEXCEPTION_POINTERS pExInfo)
{
	CreateDumpFile(pExInfo);
	
	CUIMApplication::GetApplication()->ExecuteCommand(
			new COpenBugReportCommand(NULL, m_strDumpFile));
}


BOOL CCrashReportHelper::CreateDumpFile(PEXCEPTION_POINTERS pExInfo)
{
	HANDLE hFile = NULL;
	CTime currTime = CTime::GetCurrentTime();
	CFileVersion fileVersion(CUtility::GetAppFullPathName());
	CString	strAppVersion;
	
	// Build the dump file name.
	//
	ULONGLONG llVersion = fileVersion.GetVersion();
	USHORT* pVersion = reinterpret_cast<USHORT*>(&llVersion);
	
	strAppVersion.Format(_T("%d.%d.%d.%d"), pVersion[3], pVersion[2], pVersion[1], pVersion[0]);
	
	m_strDumpFile.Format("%s%s_%s_%s.dmp", 
			CUtility::GetUIMFolder(), 
			currTime.Format("%Y-%m-%d_%H-%M-%S"), 
			strAppVersion, 
			CUtility::GetAppName());
	
	// Create the file.
	//
	hFile = CreateFile(
			m_strDumpFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	// Write the minidump to the file.
	//
	if (hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = pExInfo;
		eInfo.ClientPointers = FALSE;
		
		//MINIDUMP_CALLBACK_INFORMATION cbMiniDump;
		//cbMiniDump.CallbackRoutine = CExceptionReport::miniDumpCallback;
		//cbMiniDump.CallbackParam = 0;
		
		MiniDumpWriteDump(
				GetCurrentProcess(),
				GetCurrentProcessId(),
				hFile,
				MiniDumpNormal,
				pExInfo ? &eInfo : NULL,
				NULL,
				NULL/*&cbMiniDump*/);
		
		// Close file.
		//
		CloseHandle(hFile);

		return TRUE;
	}
	
	return FALSE;
}

