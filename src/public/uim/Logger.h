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

//#include "atlfile.h"
#include <iostream>
#include <fstream>
#include "interfaces/IUOLMessengerLogger.h"


using namespace std;


class CLogger : public IUOLMessengerLogger2
{
public:
	CLogger(CString strModuleName, CString strIniFileName, CString strLogFileName);
	virtual ~CLogger();

	virtual BOOL Initialize(BOOL bSimplifiedStartRecord);
	virtual BOOL Initialize(BOOL bSimplifiedStartRecord, 
			LogLevelType uiLogLevel, int iPurgeInterval);
	virtual void Finalize();
	
	virtual HRESULT LogStartMethod(CString strMethod);
	virtual HRESULT LogEndMethod(CString strMethod);
	virtual HRESULT LogInfo(CString strMethod, CString strInfo);
	virtual HRESULT LogDebug(CString strMethod, CString strFormatDebug, ...);
	virtual HRESULT LogWarning(CString strMethod, CString strFormatWarning, ...);
	virtual HRESULT LogError(CString strMethod, CString strFormatError, ...);
	
	virtual HRESULT LogErrorToFile(CString strFilePath, 
			CString strLogMessage, BOOL bTruncate, LogLevelType uLogLevelType);
	
	/*
	 * If we desire only WARNING and ERROR records, we inform only the WARNING id 
	 * on parameter.
	 * If GLOBAL_LOGGER_LOG_LEVEL_DEBUG id is informed, DEBUG, INFO, WARNING 
	 * and ERROR records will be returned.
	 * The id informed is cumulative with higher level id's.
	 */
	CString GetLogsFromLevel(int iMinimmumLevel, 
			const CAtlList<CString>& filterPatternsList);
	void GetLogsFromLevelAsList(int iMinimmumLevel, 
			const CAtlList<CString>& filterPatternsList, 
			CAtlList<CString>& logsList);
	
protected:	
	HRESULT WriteAndFlush(ofstream& writeFile, 
			CString strFilePath, CString strToWrite);
	
	virtual HRESULT Log(CString strToLog, BOOL bWithTime = TRUE, BOOL bLineBreak = TRUE);
	
	virtual CString GetFormatedDateTime();
	virtual CString GetFormatedDate(time_t formatingTime = NULL);
	virtual CString GetFormatedTime(time_t formatingTime = NULL);
	
	virtual LogLevelType GetLogLevel();
	
	virtual CString GetLogLevelName(LogLevelType uLogLevelType);
	
	CString GetGlobalLogFileContent();
	
	CString GetDefaultLogLevelName();
	void GetLogLevelsListFromLevelId(int iMinimmumLevel, CAtlList<CString>& levelsList);

	LogLevelType GetLogLevelId(CString strLogLevelName);
	
	HRESULT LogInitialRecord();
	
	BOOL MatchRecordWithFilter(CString strLogRecord, 
			const CAtlList<CString>& filterPatternsList);
	
protected:	
	BOOL		m_bIsLogRunning;
	
	CString		m_strModuleName;
	CString		m_strIniFileName;
	CString		m_strLogFileName;
	CString		m_strTimeFormat;
	CString		m_strLoggerKeyRoot;
	CString		m_strLoggerKeyActive;
	CString		m_strLoggerKeyLevel;
	CString		m_strLoggerKeyPurgeInterval;
	CString		m_strLoggerKeyLastPurge;
	
	ofstream	m_logFile;
	
	int			m_iLogLevel;
	
	CComAutoCriticalSection		m_csLogFile;
};

MAKEAUTOPTR(CLogger);

