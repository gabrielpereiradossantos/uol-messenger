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
#include "resource.h"
#include "./Logger.h"

#include <MinDependencySpecialFolder.h>


#define			STR_DATE_TIME_BUFFER_SIZE				64
#define			STR_FORMAT_BUFFER_MAX_SIZE				2048	// Is this enough ???
#define			GLOBAL_LOGGER_DEFAULT_PURGE_INTERVAL	5


// Internal log id.
//
#define			GLOBAL_LOGGER_LOG_LEVEL_ESSENTIAL		0

#define			GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE		"TRACE"
#define			GLOBAL_LOGGER_LOG_LEVEL_STR_DEBUG		"DEBUG"
#define			GLOBAL_LOGGER_LOG_LEVEL_STR_INFO		"INFO"
#define			GLOBAL_LOGGER_LOG_LEVEL_STR_WARNING		"WARNING"
#define			GLOBAL_LOGGER_LOG_LEVEL_STR_ERROR		"ERROR"
#define			GLOBAL_LOGGER_LOG_LEVEL_STR_ESSENTIAL	"ESSENTIAL"



CLogger::CLogger(CString strModuleName, CString strIniFileName, CString strLogFileName) : 
		m_strModuleName(strModuleName), 
		m_strIniFileName(strIniFileName), 
		m_strLogFileName(strLogFileName), 
		m_bIsLogRunning(FALSE)
{
	m_strLoggerKeyRoot = m_strModuleName;
	m_strLoggerKeyActive = m_strModuleName + "LogActive";
	m_strLoggerKeyLevel = "LogLevel";
	m_strLoggerKeyPurgeInterval = "PurgeInterval";
	m_strLoggerKeyLastPurge = "LastPurge";
}


CLogger::~CLogger()
{
}


BOOL CLogger::Initialize(BOOL bSimplifiedStartRecord)
{
	HRESULT hResult = E_FAIL;
	char strLogLevel[512];
	ios_base::openmode OpenMode = ios::out | ios::app;
	BOOL bSaveLastPurge = FALSE;
	
	UINT bLogActive = GetPrivateProfileInt(m_strLoggerKeyRoot, 
			m_strLoggerKeyActive, 0, m_strIniFileName);
	
	if (bLogActive)
	{
		memset(strLogLevel, 0, 512);
		GetPrivateProfileString(m_strLoggerKeyRoot, 
			m_strLoggerKeyLevel, GetDefaultLogLevelName(), 
			strLogLevel, 512, m_strIniFileName);
		
		m_iLogLevel = GetLogLevelId(strLogLevel);
		
		UINT iPurgeInterval = GetPrivateProfileInt(m_strLoggerKeyRoot, 
			m_strLoggerKeyPurgeInterval, 
			GLOBAL_LOGGER_DEFAULT_PURGE_INTERVAL, m_strIniFileName);

		UINT iLastPurge = GetPrivateProfileInt(m_strLoggerKeyRoot, 
			m_strLoggerKeyLastPurge, 0, m_strIniFileName);

		if (iLastPurge > 0)
		{
			long lNextPurgeTime = iLastPurge + (iPurgeInterval * 86400);

			if (time(NULL) > lNextPurgeTime)
			{
				OpenMode = ios::out;

				bSaveLastPurge = TRUE;
			}
		}
		else
		{
			bSaveLastPurge = TRUE;
		}

		if (bSaveLastPurge)
		{
			CString strLastPurge;
			strLastPurge.Format("%d", time(NULL));

			WritePrivateProfileString(m_strLoggerKeyRoot, 
					m_strLoggerKeyLastPurge, strLastPurge, m_strIniFileName);
		}

		m_logFile.open(m_strLogFileName, OpenMode);

		if (m_logFile.good())
		{
			m_bIsLogRunning = TRUE;
			
			if (bSimplifiedStartRecord)
			{
				// Log simplified message as ESSENTIAL (higher level) so it 
				// can be always caught during records listing.
				//
				CString strMessage;
				strMessage.Format("(%s) - %s - Initializing log session for %s ...", 
						GetFormatedDateTime(), 
						GLOBAL_LOGGER_LOG_LEVEL_STR_ESSENTIAL, 
						m_strModuleName);
				
				hResult = Log(strMessage, FALSE);
			}
			else
			{
				hResult = LogInitialRecord();
			}
		}
		else
		{
			m_bIsLogRunning = FALSE;

			// No other option, but show the error up.
			//
			CString strError;
			strError.Format("Falha ao abrir o arquivo de log [%s].", m_strLogFileName);		
			MessageBox(NULL, strError, "UIM", MB_OK);
		}
	}

	return SUCCEEDED(hResult);
}


BOOL CLogger::Initialize(BOOL bSimplifiedStartRecord, 
						 LogLevelType uiLogLevel, int iPurgeInterval)
{
	CString strLogLevel = GetLogLevelName(uiLogLevel);
	
	CString strPurgeInterval;
	strPurgeInterval.Format("%d", iPurgeInterval);
	
	WritePrivateProfileString(m_strLoggerKeyRoot, 
			m_strLoggerKeyActive, "1", m_strIniFileName);
	
	WritePrivateProfileString(m_strLoggerKeyRoot, 
			m_strLoggerKeyLevel, strLogLevel, m_strIniFileName);
	
	WritePrivateProfileString(m_strLoggerKeyRoot, 
			m_strLoggerKeyPurgeInterval, strPurgeInterval, m_strIniFileName);
	
	return Initialize(bSimplifiedStartRecord);
}


void CLogger::Finalize()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	if (m_bIsLogRunning)
	{
		m_bIsLogRunning = FALSE;
		
		m_logFile.close();
	}
}


HRESULT CLogger::LogStartMethod(CString strMethod)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	HRESULT hResult = E_FAIL;
	CString strMessage;
	
	if ( (m_bIsLogRunning) && 
			(m_iLogLevel >= GLOBAL_LOGGER_LOG_LEVEL_TRACE) )
	{
		strMessage.Format("(%s) - Method [%s] - %s - Entering ...\r\n", 
				GetFormatedDateTime(), strMethod, GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE);
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::LogEndMethod(CString strMethod)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	HRESULT hResult = E_FAIL;
	CString strMessage;
	
	if ( (m_bIsLogRunning) && 
			(m_iLogLevel >= GLOBAL_LOGGER_LOG_LEVEL_TRACE) )
	{
		strMessage.Format("(%s) - Method [%s] - %s - Leaving ...\r\n", 
				GetFormatedDateTime(), strMethod, GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE);
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::LogInfo(CString strMethod, CString strInfo)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	HRESULT hResult = E_FAIL;
	CString strMessage;
	
	if ( (m_bIsLogRunning) && 
			(m_iLogLevel >= GLOBAL_LOGGER_LOG_LEVEL_INFO) )
	{
		strMessage.Format("(%s) - Method [%s] - %s - %s\r\n", GetFormatedDateTime(), 
				strMethod, GLOBAL_LOGGER_LOG_LEVEL_STR_INFO, strInfo);
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::LogDebug(CString strMethod, CString strFormatDebug, ...)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	HRESULT hResult = E_FAIL;
	va_list vaList;
	CString strMessage;
	
	if ( (m_bIsLogRunning) && 
			(m_iLogLevel >= GLOBAL_LOGGER_LOG_LEVEL_DEBUG) )
	{
		char debugBuffer[STR_FORMAT_BUFFER_MAX_SIZE];
		memset(debugBuffer, 0, STR_FORMAT_BUFFER_MAX_SIZE);
		
		va_start(vaList, strFormatDebug);
		vsprintf(debugBuffer, strFormatDebug, vaList);
		va_end(vaList);
		
		strMessage.Format("(%s) - Method [%s] - %s - %s\r\n", GetFormatedDateTime(), 
				strMethod, GLOBAL_LOGGER_LOG_LEVEL_STR_DEBUG, debugBuffer);
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::LogWarning(CString strMethod, CString strFormatWarning, ...)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	HRESULT hResult = E_FAIL;
	va_list vaList;
	CString strMessage;
	
	if ( (m_bIsLogRunning) && 
			(m_iLogLevel >= GLOBAL_LOGGER_LOG_LEVEL_WARNING) )
	{
		char warningBuffer[STR_FORMAT_BUFFER_MAX_SIZE];
		memset(warningBuffer, 0, STR_FORMAT_BUFFER_MAX_SIZE);
		
		va_start(vaList, strFormatWarning);
		vsprintf(warningBuffer, strFormatWarning, vaList);
		va_end(vaList);
		
		strMessage.Format("(%s) - Method [%s] - %s - %s\r\n", GetFormatedDateTime(), 
				strMethod, GLOBAL_LOGGER_LOG_LEVEL_STR_WARNING, warningBuffer);
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::LogError(CString strMethod, CString strFormatError, ...)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	HRESULT hResult = E_FAIL;
	va_list vaList;
	CString strMessage;
	
	if ( (m_bIsLogRunning) && 
			(m_iLogLevel >= GLOBAL_LOGGER_LOG_LEVEL_ERROR) )
	{
		char errorBuffer[STR_FORMAT_BUFFER_MAX_SIZE];
		memset(errorBuffer, 0, STR_FORMAT_BUFFER_MAX_SIZE);
		
		va_start(vaList, strFormatError);
		vsprintf(errorBuffer, strFormatError, vaList);
		va_end(vaList);
		
		strMessage.Format("(%s) - Method [%s] - %s - %s\r\n", GetFormatedDateTime(), 
				strMethod, GLOBAL_LOGGER_LOG_LEVEL_STR_ERROR, errorBuffer);
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::LogErrorToFile(CString strFilePath, 
		CString strLogMessage, BOOL bTruncate, LogLevelType uLogLevelType)
{
	HRESULT hResult = E_FAIL;
	ofstream logFile;
	ios_base::openmode OpenMode = ios::out | ios::app;
	CString strFormatedLogMessage;
	
	if (bTruncate)
	{
		OpenMode = ios::out;
	}
	
	logFile.open(strFilePath, OpenMode);
	
	if (logFile.is_open())
	{
		strFormatedLogMessage.Format("(%s) - %s - %s", 
				GetFormatedDateTime(), GetLogLevelName(uLogLevelType), strLogMessage);
		
		hResult = WriteAndFlush(logFile, strFilePath, strFormatedLogMessage);
	}
	else
	{
		MessageBox(NULL, "Falha ao abrir o arquivo de log: " + strFilePath, "Erro!", MB_OK);
	}
	
	logFile.close();
	
	return hResult;
}


CString CLogger::GetLogsFromLevel(int iMinimmumLevel, 
								  const CAtlList<CString>& filterPatternsList)
{
	CString logsRecords;
	CString strFileContent;
	CAtlList<CString> levelsList;
	
	strFileContent = GetGlobalLogFileContent();
	
	if (!strFileContent.IsEmpty())
	{
		CString strToken;
		int nPos = 0;
		strToken = strFileContent.Tokenize("\r\n", nPos);
		
		if (!strToken.IsEmpty())
		{
			GetLogLevelsListFromLevelId(iMinimmumLevel, levelsList);
		}
		
		while (strToken != "")
		{
			CString strFindingText;
			BOOL bMatched = FALSE;
			
			for (POSITION listPos = levelsList.GetHeadPosition(); listPos; )
			{
				strFindingText.Format("- %s -", levelsList.GetNext(listPos));
				
				if (strToken.Find(strFindingText) != -1)
				{
					logsRecords += strToken + "\n";
					
					break;
				}
			}
			
			if ( (!bMatched) && (MatchRecordWithFilter(strToken, filterPatternsList)) )
			{
				logsRecords += strToken + "\n";
			}
			
			strToken = strFileContent.Tokenize("\r\n", nPos);
		}
	}
	
	return logsRecords;
}


void CLogger::GetLogsFromLevelAsList(int iMinimmumLevel, 
									 const CAtlList<CString>& filterPatternsList, 
									 CAtlList<CString>& logsList)
{
	CString strFileContent;
	CAtlList<CString> levelsList;
	
	logsList.RemoveAll();
	
	strFileContent = GetGlobalLogFileContent();
	
	if (!strFileContent.IsEmpty())
	{
		CString strToken;
		int nPos = 0;
		strToken = strFileContent.Tokenize("\r\n", nPos);
		
		if (!strToken.IsEmpty())
		{
			GetLogLevelsListFromLevelId(iMinimmumLevel, levelsList);
		}
		
		while (strToken != "")
		{
			CString strFindingText;
			BOOL bMatched = FALSE;
			
			for (POSITION listPos = levelsList.GetHeadPosition(); listPos; )
			{
				strFindingText.Format("- %s -", levelsList.GetNext(listPos));
				
				if (strToken.Find(strFindingText) != -1)
				{
					logsList.AddTail(strToken);
					
					bMatched = TRUE;
					
					break;
				}
			}
			
			if ( (!bMatched) && (MatchRecordWithFilter(strToken, filterPatternsList)) )
			{
				logsList.AddTail(strToken);
			}
			
			strToken = strFileContent.Tokenize("\r\n", nPos);
		}
	}
}


HRESULT CLogger::Log(CString strToLog, BOOL bWithTime, BOOL bLineBreak)
{
	HRESULT hResult = E_FAIL;
	CString strMessage;
	CString strLineBreak = bLineBreak ? "\r\n" : "";
	
	if (m_bIsLogRunning)
	{
		if (bWithTime)
		{
			strMessage.Format("(%s) - %s%s", GetFormatedDateTime(), strToLog, strLineBreak);
		}
		else
		{
			strMessage.Format("%s%s", strToLog, strLineBreak);
		}
		
		hResult = WriteAndFlush(m_logFile, m_strLogFileName, strMessage);
	}
	
	return hResult;
}


HRESULT CLogger::WriteAndFlush(ofstream& writeFile, 
		CString strFilePath, CString strToWrite)
{
	HRESULT hResult = E_FAIL;

	if (writeFile.is_open())
	{		
		CString strError;	

		if (writeFile.write(strToWrite.GetBuffer(), strToWrite.GetLength()))
		{
			if (writeFile.flush())
			{
				hResult = S_OK;
			}
			else
			{
				// No other option, but show the error up.
				//
				strError.Format("Falha ao realizar o flush de [%s] no arquivo de log [%s].", 
						strToWrite, strFilePath);

				MessageBox(NULL, strError, "UIM", MB_OK);
			}
		}
		else
		{
			// No other option, but show the error up.
			//
			strError.Format("Falha ao escrever [%s] no arquivo de log [%s].", 
					strToWrite, strFilePath);

			MessageBox(NULL, strError, "UIM", MB_OK);
		}
	}
	
	return hResult;
}


CString CLogger::GetFormatedDateTime()
{
	CString strFormatedDateTime;
	
	time_t currTime = time(NULL);
	
	strFormatedDateTime.Format("%s - %s", GetFormatedDate(currTime), GetFormatedTime(currTime));
	
	return strFormatedDateTime;
}


CString CLogger::GetFormatedDate(time_t formatingTime)
{
	CString strFormatedDate;
	time_t currTime = NULL;
	
	if (formatingTime)
	{
		currTime = formatingTime;
	}
	else
	{
		currTime = time(NULL);
	}
	
	char strDest[STR_DATE_TIME_BUFFER_SIZE];
	memset(strDest, 0, STR_DATE_TIME_BUFFER_SIZE);
	
	if (strftime(strDest, STR_DATE_TIME_BUFFER_SIZE - 1, "%d/%m/%Y", localtime(&currTime)))
	{
		strFormatedDate.Format("%s", strDest);
	}
	
	return strFormatedDate;
}


CString CLogger::GetFormatedTime(time_t formatingTime)
{
	CString strFormatedTime;
	time_t currTime = NULL;
	
	if (formatingTime)
	{
		currTime = formatingTime;
	}
	else
	{
		currTime = time(NULL);
	}
	
	char strDest[STR_DATE_TIME_BUFFER_SIZE];
	memset(strDest, 0, STR_DATE_TIME_BUFFER_SIZE);
	
	if (strftime(strDest, STR_DATE_TIME_BUFFER_SIZE - 1, "%H:%M:%S", localtime(&currTime)))
	{
		strFormatedTime.Format("%s", strDest);
	}
	
	return strFormatedTime;
}


CString CLogger::GetGlobalLogFileContent()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csLogFile);
	
	std::string strLine;
	CString strFileContent;

	ifstream logFile;
	logFile.open(m_strLogFileName);
	
	if (logFile.is_open())
	{
		logFile.seekg(0,ifstream::beg);

		while (!logFile.eof())
		{
			getline(logFile, strLine);
			strFileContent.Append(strLine.c_str());
		}
		logFile.close();
	}

	return strFileContent;
}


CString CLogger::GetDefaultLogLevelName()
{
	return GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE;
}


IUOLMessengerLogger::LogLevelType CLogger::GetLogLevel()
{
	return (IUOLMessengerLogger::LogLevelType)m_iLogLevel;
}


CString CLogger::GetLogLevelName(LogLevelType uLogLevelType)
{
	CString strLogLevelName;
	
	switch (uLogLevelType)
	{
		case GLOBAL_LOGGER_LOG_LEVEL_TRACE:
			
			strLogLevelName = GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE;
			
		break;
		
		case GLOBAL_LOGGER_LOG_LEVEL_DEBUG:
			
			strLogLevelName = GLOBAL_LOGGER_LOG_LEVEL_STR_DEBUG;
			
		break;
		
		case GLOBAL_LOGGER_LOG_LEVEL_INFO:
			
			strLogLevelName = GLOBAL_LOGGER_LOG_LEVEL_STR_INFO;
			
		break;
		
		case GLOBAL_LOGGER_LOG_LEVEL_WARNING:
			
			strLogLevelName = GLOBAL_LOGGER_LOG_LEVEL_STR_WARNING;
			
		break;
		
		case GLOBAL_LOGGER_LOG_LEVEL_ERROR:
			
			strLogLevelName = GLOBAL_LOGGER_LOG_LEVEL_STR_ERROR;
			
		break;
		
		case GLOBAL_LOGGER_LOG_LEVEL_ESSENTIAL:
			
			strLogLevelName = GLOBAL_LOGGER_LOG_LEVEL_STR_ESSENTIAL;
			
		break;
	}
	
	return strLogLevelName;
}


void CLogger::GetLogLevelsListFromLevelId(
		int iMinimmumLevel, CAtlList<CString>& levelsList)
{
	levelsList.RemoveAll();
	
	switch (iMinimmumLevel)
	{
		case GLOBAL_LOGGER_LOG_LEVEL_TRACE:
			
			levelsList.AddTail(GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE);
			
		case GLOBAL_LOGGER_LOG_LEVEL_DEBUG:
			
			levelsList.AddTail(GLOBAL_LOGGER_LOG_LEVEL_STR_DEBUG);
			
		case GLOBAL_LOGGER_LOG_LEVEL_INFO:
			
			levelsList.AddTail(GLOBAL_LOGGER_LOG_LEVEL_STR_INFO);
			
		case GLOBAL_LOGGER_LOG_LEVEL_WARNING:
			
			levelsList.AddTail(GLOBAL_LOGGER_LOG_LEVEL_STR_WARNING);
			
		case GLOBAL_LOGGER_LOG_LEVEL_ERROR:
			
			levelsList.AddTail(GLOBAL_LOGGER_LOG_LEVEL_STR_ERROR);
			
		case GLOBAL_LOGGER_LOG_LEVEL_ESSENTIAL:
			
			levelsList.AddTail(GLOBAL_LOGGER_LOG_LEVEL_STR_ESSENTIAL);
			
		break;
	}
}


IUOLMessengerLogger::LogLevelType CLogger::GetLogLevelId(CString strLogLevelName)
{
	LogLevelType uiLogLevel = GLOBAL_LOGGER_LOG_LEVEL_WARNING;
	
	if (strLogLevelName.CompareNoCase(GLOBAL_LOGGER_LOG_LEVEL_STR_TRACE) == 0)
	{
		uiLogLevel = GLOBAL_LOGGER_LOG_LEVEL_TRACE;
	}
	else if (strLogLevelName.CompareNoCase(GLOBAL_LOGGER_LOG_LEVEL_STR_DEBUG) == 0)
	{
		uiLogLevel = GLOBAL_LOGGER_LOG_LEVEL_DEBUG;
	}
	else if (strLogLevelName.CompareNoCase(GLOBAL_LOGGER_LOG_LEVEL_STR_INFO) == 0)
	{
		uiLogLevel = GLOBAL_LOGGER_LOG_LEVEL_INFO;
	}
	else if (strLogLevelName.CompareNoCase(GLOBAL_LOGGER_LOG_LEVEL_STR_WARNING) == 0)
	{
		uiLogLevel = GLOBAL_LOGGER_LOG_LEVEL_WARNING;
	}
	else if (strLogLevelName.CompareNoCase(GLOBAL_LOGGER_LOG_LEVEL_STR_ERROR) == 0)
	{
		uiLogLevel = GLOBAL_LOGGER_LOG_LEVEL_ERROR;
	}
	
	return uiLogLevel;
}


HRESULT CLogger::LogInitialRecord()
{
	HRESULT hResult = E_FAIL;
	
	hResult = Log("\r\n\r\n================================================================================", FALSE);
	
	if (SUCCEEDED(hResult))
	{
		CString strMessage;
		strMessage.Format("%s ---> Initializing log session for %s ...", 
				GetFormatedDateTime(), m_strModuleName);
		
		hResult = Log(strMessage, FALSE);
	}
	
	if (SUCCEEDED(hResult))
	{
		hResult = Log("\r\n================================================================================\r\n", FALSE);
	}
	
	return hResult;
}


BOOL CLogger::MatchRecordWithFilter(CString strLogRecord, 
		const CAtlList<CString>& filterPatternsList)
{
	BOOL bMatched = FALSE;
	
	for (POSITION listPos = filterPatternsList.GetHeadPosition(); listPos; )
	{
		bMatched = strLogRecord.Find(filterPatternsList.GetNext(listPos)) != -1;
		
		if (bMatched)
		{
			break;
		}
	}
	
	return bMatched;
}

