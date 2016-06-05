/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Check http://www.codeproject.com/debug/postmortemdebug_standalone1.asp
 */

#include <memorydump/MSVCMemoryDump.h>

#include <util/Logger.h>
#include <util/Path.h>

#include <shlwapi.h>

#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>

static const std::string DBGHELP_DLL = "dbghelp.dll";

MSVCMemoryDump::MSVCMemoryDump(const std::string & applicationName, const std::string & revision)
	: MemoryDump(applicationName, revision) {

	SetUnhandledExceptionFilter(topLevelFilter);
}

MSVCMemoryDump::~MSVCMemoryDump() {
}

HMODULE MSVCMemoryDump::loadDebugHelpLibrary() {
	//Firstly see if dbghelp.dll is around and has the function we need
	//look next to the .exe first, as the one in the system32 directory
	//might be old (e.g. Windows 2000)

	std::string dbghelpPath = Path::getApplicationDirPath();
	dbghelpPath += DBGHELP_DLL;
	HMODULE hDll = LoadLibraryA(dbghelpPath.c_str());
	if (hDll) {
		LOG_DEBUG(DBGHELP_DLL + " loaded from current directory");
	}

	if (!hDll) {
		//Load any version we can
		hDll = LoadLibraryA(DBGHELP_DLL.c_str());
		LOG_DEBUG(DBGHELP_DLL + " loaded from the system");
	}

	if (!hDll) {
		LOG_ERROR(DBGHELP_DLL + " not found");
	}

	return hDll;
}

MINIDUMPWRITEDUMP MSVCMemoryDump::loadMiniDumpWriteDumpFunction() {
	MINIDUMPWRITEDUMP pDump = NULL;
	HMODULE hDll = loadDebugHelpLibrary();
	if (hDll) {
		pDump = (MINIDUMPWRITEDUMP) GetProcAddress(hDll, "MiniDumpWriteDump");
		if (!pDump) {
			LOG_ERROR(DBGHELP_DLL + " too old");
		}
	}

	return pDump;
}

long MSVCMemoryDump::topLevelFilter(struct _EXCEPTION_POINTERS * pExceptionInfo) {
	long ret = EXCEPTION_CONTINUE_SEARCH;

	MINIDUMPWRITEDUMP pDump = loadMiniDumpWriteDumpFunction();
	if (pDump) {
		//Waits a bit because several minidump can be created
		Sleep(1000);

		//Name of the memory dump
		//Use current path
		std::string memoryDumpName;
		memoryDumpName += _applicationName;
		memoryDumpName += "-rev";
		memoryDumpName += _revision;
		memoryDumpName += "-";
		memoryDumpName += getCurrentDateTime();
		memoryDumpName += ".dmp";

		//GetModuleFileName retrieves the path of the executable file of the current process.
		std::string memoryDumpFile(Path::getApplicationDirPath());
		memoryDumpFile += memoryDumpName;

		//Creates the file
		HANDLE hFile = CreateFileA(memoryDumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE) {
			_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

			ExInfo.ThreadId = GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExceptionInfo;
			ExInfo.ClientPointers = NULL;

			//Writes the dump
			BOOL ok = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
			if (ok) {
				LOG_DEBUG("dump file saved to: " + memoryDumpName);
				ret = EXCEPTION_EXECUTE_HANDLER;
			} else {
				LOG_ERROR("failed to save dump file to: " + memoryDumpName + " " + String::fromNumber(GetLastError()));
			}
			CloseHandle(hFile);
		} else {
			LOG_ERROR("failed to create dump file: " + memoryDumpName + " " + String::fromNumber(GetLastError()));
		}

		//Launches crashreport.exe
		std::string commandLine = "owcrashreport";
		if (!_styleName.empty()) {
			commandLine += " -style=";
			commandLine += _styleName;
		}
		commandLine += " -d ";
		commandLine += "\"";
		commandLine += memoryDumpFile;
		commandLine += "\"";
		commandLine += " -n ";
		commandLine += "\"";
		commandLine += _applicationName;
		commandLine += "\"";
		if (!_languageFilename.empty()) {
			commandLine += " -l ";
			commandLine += "\"";
			commandLine += _languageFilename;
			commandLine += "\"";
		}
		if (getAdditionalInfo) {
			commandLine += " -i ";
			commandLine += "\"";
			commandLine += getAdditionalInfo();
			commandLine += "\"";
		}

		//Flushes the logger file
		Logger::logger.flush();

		executeProcess(commandLine);
	}

	return ret;
}

BOOL MSVCMemoryDump::executeProcess(const std::string & commandLine) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	GetStartupInfoA(&si);

	BOOL success = CreateProcessA(NULL,
		(char *) commandLine.c_str(),	//Name of app to launch
		NULL,	//Default process security attributes
		NULL,	//Default thread security attributes
		FALSE,	//Don't inherit handles from the parent
		0,	//Normal priority
		NULL,	//Use the same environment as the parent
		NULL,	//Launch in the current directory
		&si,	//Startup Information
		&pi);	//Process information stored upon return

	return success;
}
