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
#include "./PurpleLogger.h"
#include "../protocols/improtocolfactory.h"


CPurpleLogger::CPurpleLogger(CString strModuleName, CString strIniFileName, CString strLogFileName) : 
		CLogger(strModuleName, strIniFileName, strLogFileName)
{
	m_iLogLevel = GLOBAL_LOGGER_LOG_LEVEL_TRACE;
}


CPurpleLogger::~CPurpleLogger()
{
}


BOOL CPurpleLogger::Initialize(BOOL bSimplifiedStartRecord)
{
	HRESULT hResult = E_FAIL;
	
	if (__super::Initialize(bSimplifiedStartRecord))
	{
		hResult = S_OK;
		
		CIMProtocolFactory::GetInstance()->SetPurpleDebugLogger(this);
		
		CIMProtocolFactory::GetInstance()->EnablePurpleDebugLog(m_bIsLogRunning);
	}
	
	return (SUCCEEDED(hResult));
}


// Get UIM Debug Level from Purple Debug Level
IUOLMessengerLogger::LogLevelType CPurpleLogger::GetUIMDebugLevel(int iLogLevel)
{
	switch (iLogLevel) 
	{
	case 0:
		return GLOBAL_LOGGER_LOG_LEVEL_TRACE;
		break;
	case 1:
		return GLOBAL_LOGGER_LOG_LEVEL_DEBUG; 
		break;
	case 2:
		return GLOBAL_LOGGER_LOG_LEVEL_INFO;
		break;
	case 3:
		return GLOBAL_LOGGER_LOG_LEVEL_WARNING;
		break;
	case 4:
		return GLOBAL_LOGGER_LOG_LEVEL_ERROR;
		break;
	default:
		return GLOBAL_LOGGER_LOG_LEVEL_WARNING;
	}
	 
}


void CPurpleLogger::DebugLog(int iLevel, const char* pszCategory, const char* pszText)
{
	if (GetUIMDebugLevel(iLevel) <= m_iLogLevel)
	{
		CString strDebugLog;
		CString strTextNoBreakLines(pszText);
		
		strTextNoBreakLines.Replace("\n", " ");
		strTextNoBreakLines.Replace("\r", " ");
		
		if (pszCategory != NULL)
		{
			strDebugLog.Format("%s - %s: %s", 
					GetLogLevelName(GetUIMDebugLevel(iLevel)), 
					pszCategory, strTextNoBreakLines);
		}
		else
		{
			strDebugLog.Format("%s - %s", 
					GetLogLevelName(GetUIMDebugLevel(iLevel)), 
					strTextNoBreakLines);
		}
		
		Log(strDebugLog, TRUE, TRUE);
	}
}