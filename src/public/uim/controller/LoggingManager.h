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

#include <interfaces/IUOLMessengerLoggingManager.h>

#include "../UIMGlobalLogger.h"
#include "../PurpleLogger.h"


class CReportableLogInfo;

MAKEAUTOPTR(CReportableLogInfo);


class CLoggingManager : 
	public IUOLMessengerLoggingManager
{
public:
	CLoggingManager();
	virtual ~CLoggingManager();
	
	virtual BOOL Init();
	virtual void Finalize();
	
	IUOLMessengerLogger2Ptr GetGlobalLogger();
	IUOLMessengerLogger2Ptr GetPurpleLogger();
	
	void GetReportableLogsList(CAtlList<CReportableLogInfoPtr>& reportableLogsList);
	
protected:
	
	// IUOLMessengerLoggingManager interface.
	//
	virtual IUOLMessengerLogger2Ptr RegisterLogger(const CString& strModuleName, 
			const CString& strIniFilePath, const CString& strLogFilePath, 
			IUOLMessengerLogger::LogLevelType uiLogLevel = 
					IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING, 
			int iPurgeInterval = 5, BOOL bSimplifiedStartRecord = FALSE);
	
	virtual BOOL UnregisterLogger(const CString& strModuleName);
	
	virtual IUOLMessengerLogger2Ptr GetLogger(const CString& strModuleName);
	
	virtual CString GetLogsDefaultPath() const;
	
	virtual BOOL AddLogInfoToReports(const CString& strModuleName, 
			IUOLMessengerLogger::LogLevelType uiLogLevel, 
			const CAtlList<CString>* pFilterPatternsList, 
			const CString& strReportTitle);
	
	virtual BOOL RemoveLogInfoFromReports(const CString& strModuleName);
	
private:
	
	static const char* UIM_LOGGER_MODULE_NAME;
	static const char* PURPLE_LOGGER_MODULE_NAME;
	
private:
	
	CString					m_strInternalIniFileName;
	CString					m_strUIMLogFileName;
	CString					m_strPurpleLogFileName;
	
	typedef CAtlMap<CString, IUOLMessengerLogger2Ptr>	CLoggerMap;
	CLoggerMap				m_loggerMap;
	
	typedef CAtlMap<CString, CReportableLogInfoPtr>		CReportableLogsMap;
	CReportableLogsMap		m_reportableLogsMap;
};

MAKEAUTOPTR(CLoggingManager);


class CReportableLogInfo
{
public:
	
	CReportableLogInfo(CString strLogId, 
			IUOLMessengerLogger::LogLevelType uiLogLevel, 
			const CAtlList<CString>* pFilterPatternsList, 
			CString strReportTitle);
	
	virtual ~CReportableLogInfo();
	
	CString GetLogId();
	IUOLMessengerLogger::LogLevelType GetLogLevel();
	void GetFilterPatternsList(CAtlList<CString>& filterPatternsList);
	CString GetReportTitle();
	
private:
	
	CString				m_strLogId;
	IUOLMessengerLogger::LogLevelType	m_uiLogLevel;
	CAtlList<CString>	m_filterPatternsList;
	CString				m_strReportTitle;
};

