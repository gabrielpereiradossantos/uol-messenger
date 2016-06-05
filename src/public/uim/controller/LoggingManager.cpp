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
#include "LoggingManager.h"

#include "./UIMApplication.h"
#include <MinDependencySpecialFolder.h>
#include "../FileUtils.h"


#define			INTERNAL_LOGGERS_INI_FILE_NAME				"UIMLog.ini"

#define			GLOBAL_LOGGER_LOG_FILE_NAME					"UIMLog.log"
#define			PURPLE_LOGGER_LOG_FILE_NAME					"PurpleLog.log"


///////////////////////////////////////////////////////////////////
// CLoggingManager
///////////////////////////////////////////////////////////////////


const char* CLoggingManager::UIM_LOGGER_MODULE_NAME = "UIM";
const char* CLoggingManager::PURPLE_LOGGER_MODULE_NAME = "Purple";


CLoggingManager::CLoggingManager()
{
}


CLoggingManager::~CLoggingManager()
{
	m_loggerMap.RemoveAll();
	m_reportableLogsMap.RemoveAll();
}


BOOL CLoggingManager::Init()
{
	BOOL bRet = FALSE;
	CLoggerPtr	pGlobalLogger;
	CLoggerPtr	pPurpleLogger;
	
	// Build the internal file names.
	//
	CMinDependencySpecialFolder	minDSF;
	CString strCompany, strProduct;
	CString strLogsRootPath;
	
	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	
	m_strInternalIniFileName.Format("%s%s\\%s\\%s", 
			minDSF.GetCommonAppData(), strCompany, strProduct, 
			INTERNAL_LOGGERS_INI_FILE_NAME);
	
	m_strUIMLogFileName.Format("%s%s\\%s\\%s", 
			minDSF.GetAppData(), strCompany, strProduct, 
			GLOBAL_LOGGER_LOG_FILE_NAME);
	
	m_strPurpleLogFileName.Format("%s%s\\%s\\%s", 
			minDSF.GetAppData(), strCompany, strProduct, 
			PURPLE_LOGGER_LOG_FILE_NAME);
	
	strLogsRootPath.Format("%s%s\\%s", 
			minDSF.GetAppData(), strCompany, strProduct);
	
	// Validates the existence or creates the main logs folder.
	//
	if (CFileUtils::CreateDirectory(strLogsRootPath))
	{
		// Initialize Global Logger.
		//
		pGlobalLogger = new CUIMGlobalLogger(UIM_LOGGER_MODULE_NAME, 
				m_strInternalIniFileName, m_strUIMLogFileName);
		
		if ( (pGlobalLogger != NULL) && 
				(pGlobalLogger->Initialize(FALSE) == TRUE) )
		{
			// Initialize Purple Logger
			pPurpleLogger = new CPurpleLogger(PURPLE_LOGGER_MODULE_NAME, 
					m_strInternalIniFileName, m_strPurpleLogFileName);
			
			if ( (pPurpleLogger != NULL) && 
					(pPurpleLogger->Initialize(FALSE) == TRUE) )
			{
				m_loggerMap.SetAt(UIM_LOGGER_MODULE_NAME, pGlobalLogger);
				m_loggerMap.SetAt(PURPLE_LOGGER_MODULE_NAME, pPurpleLogger);
				
				bRet = TRUE;
			}
		}
	}
	
	return bRet;
}


void CLoggingManager::Finalize()
{
	for (POSITION mapPos = m_loggerMap.GetStartPosition(); mapPos; )
	{
		CLoggerMap::CPair* pPair = m_loggerMap.GetNext(mapPos);
		CLogger* pLogger = NULL;
		
		if (pPair != NULL)
		{
			pLogger = dynamic_cast<CLogger*>(pPair->m_value.get());
			
			if (pLogger != NULL)
			{
				pLogger->Finalize();
			}
		}
	}
}


IUOLMessengerLogger2Ptr CLoggingManager::GetGlobalLogger()
{
	return GetLogger(UIM_LOGGER_MODULE_NAME);
}


IUOLMessengerLogger2Ptr CLoggingManager::GetPurpleLogger()
{
	return GetLogger(PURPLE_LOGGER_MODULE_NAME);
}


void CLoggingManager::GetReportableLogsList(
		CAtlList<CReportableLogInfoPtr>& reportableLogsList)
{
	reportableLogsList.RemoveAll();
	
	for (POSITION mapPos = m_reportableLogsMap.GetStartPosition(); mapPos; )
	{
		reportableLogsList.AddTail(m_reportableLogsMap.GetNext(mapPos)->m_value);
	}
}


IUOLMessengerLogger2Ptr CLoggingManager::RegisterLogger(
		const CString& strModuleName, const CString& strIniFilePath, 
		const CString& strLogFilePath, 
		IUOLMessengerLogger::LogLevelType uiLogLevel, 
		int iPurgeInterval, BOOL bSimplifiedStartRecord)
{
	CLoggerPtr pLogger = new CLogger(strModuleName, strIniFilePath, strLogFilePath);
	
	if (pLogger != NULL)
	{
		BOOL bInitializedOk = FALSE;
		
		if (ATLPath::FileExists(strIniFilePath))
		{
			bInitializedOk = pLogger->Initialize(bSimplifiedStartRecord);
		}
		else
		{
			bInitializedOk = pLogger->Initialize(bSimplifiedStartRecord, 
					uiLogLevel, iPurgeInterval);
		}
		
		if (bInitializedOk)
		{
			m_loggerMap.SetAt(strModuleName, pLogger);
		}
	}
	
	return pLogger;
}


BOOL CLoggingManager::UnregisterLogger(const CString& strModuleName)
{
	return (m_loggerMap.RemoveKey(strModuleName) == true) ? TRUE : FALSE;
}


IUOLMessengerLogger2Ptr CLoggingManager::GetLogger(const CString& strModuleName)
{
	CLoggerMap::CPair* pPair = m_loggerMap.Lookup(strModuleName);
	
	if (pPair != NULL)
	{
		return pPair->m_value;
	}
	
	return IUOLMessengerLogger2Ptr();
}


CString CLoggingManager::GetLogsDefaultPath() const
{
	CMinDependencySpecialFolder	minDSF;
	CString strCompany, strProduct, strLogsDefaultPath;
	
	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	
	strLogsDefaultPath.Format("%s%s\\%s", 
			minDSF.GetAppData(), strCompany, strProduct);
	
	return strLogsDefaultPath;
}


BOOL CLoggingManager::AddLogInfoToReports(const CString& strModuleName, 
		IUOLMessengerLogger::LogLevelType uiLogLevel, 
		const CAtlList<CString>* pFilterPatternsList, 
		const CString& strReportTitle)
{
	CLoggerMap::CPair* pPair = m_loggerMap.Lookup(strModuleName);
	BOOL bReturnOK = FALSE;
	
	if (pPair != NULL)
	{
		CReportableLogInfoPtr pReportableLogInfo = 
				new CReportableLogInfo(strModuleName, uiLogLevel, 
						pFilterPatternsList, strReportTitle);
		
		if (pReportableLogInfo != NULL)
		{
			m_reportableLogsMap.SetAt(strModuleName, pReportableLogInfo);
			
			bReturnOK = TRUE;
		}
	}
	
	return bReturnOK;
}


BOOL CLoggingManager::RemoveLogInfoFromReports(const CString& strModuleName)
{
	return (m_reportableLogsMap.RemoveKey(strModuleName) == true) ? TRUE : FALSE;
}


///////////////////////////////////////////////////////////////////
// CReportableLogInfo
///////////////////////////////////////////////////////////////////


CReportableLogInfo::CReportableLogInfo(CString strLogId, 
		IUOLMessengerLogger::LogLevelType uiLogLevel, 
		const CAtlList<CString>* pFilterPatternsList, 
		CString strReportTitle) : 
			m_strLogId(strLogId), 
			m_uiLogLevel(uiLogLevel), 
			m_strReportTitle(strReportTitle)
{
	if (pFilterPatternsList != NULL)
	{
		for (POSITION listPos = pFilterPatternsList->GetHeadPosition(); listPos; )
		{
			m_filterPatternsList.AddTail(pFilterPatternsList->GetNext(listPos));
		}
	}
}


CReportableLogInfo::~CReportableLogInfo()
{
}


CString CReportableLogInfo::GetLogId()
{
	return m_strLogId;
}


IUOLMessengerLogger::LogLevelType CReportableLogInfo::GetLogLevel()
{
	return m_uiLogLevel;
}


void CReportableLogInfo::GetFilterPatternsList(CAtlList<CString>& filterPatternsList)
{
	filterPatternsList.RemoveAll();
	
	for (POSITION listPos = m_filterPatternsList.GetHeadPosition(); listPos; )
	{
		filterPatternsList.AddTail(m_filterPatternsList.GetNext(listPos));
	}
}


CString CReportableLogInfo::GetReportTitle()
{
	return m_strReportTitle;
}

