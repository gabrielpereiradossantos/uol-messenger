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

#include "IUOLMessengerLogger.h"


class __declspec(novtable) IUOLMessengerLoggingManager
{
public:
	virtual ~IUOLMessengerLoggingManager() { }
	
	/*
	 * @param strModuleName		Works as a log ID. It's a name to identify 
	 *							the module that is logging information like 
	 *							the module names uses "UIM" and "Purple" 
	 *							used so far.
	 *							This name CAN NOT have spaces.
	 * @param strIniFilePath	Local from where we must load the config INI 
	 *							file of this logger. If the file doesn't 
	 *							exist, it'll be created considering the 
	 *							configurations informed.
	 * @param strLogFilePath	File path of log file to be used to store 
	 *							the info handled by this logger.
	 * @param uiLogLevel		The detail level of this logger.
	 * @param iPurgeInterval	The interval between purges of the info 
	 *							stored by this logger.
	 * @param bSimplifiedStartRecord	Indicates whether the initial starting 
	 *									record will have only one line (simplified), 
	 *									or more as been used until now.
	 */
	virtual IUOLMessengerLogger2Ptr RegisterLogger(const CString& strModuleName, 
			const CString& strIniFilePath, const CString& strLogFilePath, 
			IUOLMessengerLogger::LogLevelType uiLogLevel = 
					IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING, 
			int iPurgeInterval = 5, BOOL bSimplifiedStartRecord = FALSE) = 0;
	
	virtual BOOL UnregisterLogger(const CString& strModuleName) = 0;
	
	virtual IUOLMessengerLogger2Ptr GetLogger(const CString& strModuleName) = 0;
	
	virtual CString GetLogsDefaultPath() const = 0;
	
	/*
	 * AddLogInfoToReports
	 * 
	 * Add the Log records of level "uiLogLevel" and higher to 
	 * "Bug Report" and "Crash Report" with title "strReportTitle".
	 * 
	 * "pFilterPatternsList" parameter can be used so logs that are not 
	 * of "uiLogLevel" can be added to report as well, and usually 
	 * will have strings to be matched with the log records.
	 * 
	 * @return	FALSE whether the logger given by "strModuleName" was 
	 *				not registered yet, or another failure occur.
	 *			TRUE otherwise.
	 */
	virtual BOOL AddLogInfoToReports(const CString& strModuleName, 
			IUOLMessengerLogger::LogLevelType uiLogLevel, 
			const CAtlList<CString>* pFilterPatternsList, 
			const CString& strReportTitle) = 0;
	
	virtual BOOL RemoveLogInfoFromReports(const CString& strModuleName) = 0;
};

MAKEAUTOPTR(IUOLMessengerLoggingManager);

