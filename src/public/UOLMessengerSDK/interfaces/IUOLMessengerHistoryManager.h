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

#include "IUOLMessengerContact.h"
#include "IUOLMessengerHistoryContact.h"
#include "IUOLMessengerXMLManager.h"
#include "IUOLMessengerHistoryRecord.h"


typedef CAtlList<CTime> CListTimes;


class __declspec(novtable) IUOLMessengerHistoryManager
{
public:
	virtual ~IUOLMessengerHistoryManager() {}
	__declspec(deprecated) virtual BOOL SetMessageLog(IUOLMessengerContactPtr pContact, CString message, CString author) = 0;
};
MAKEAUTOPTR(IUOLMessengerHistoryManager);


class __declspec(novtable) IUOLMessengerHistoryManager2: public IUOLMessengerHistoryManager
{
public:
	virtual ~IUOLMessengerHistoryManager2() {}
	__declspec(deprecated) virtual BOOL ReadLogFilesFromFolder(CAtlMap<int, CString>& mapLog, IUOLMessengerContactPtr pContact) = 0;
	__declspec(deprecated) virtual BOOL SetConversationWindowClosed(IUOLMessengerContactPtr pContact, CTime startTime) = 0;
    __declspec(deprecated) virtual BOOL SetMessageLog(IUOLMessengerContactPtr pContact, const CString& message, const CString& author, CTime startTime) = 0;
};
MAKEAUTOPTR(IUOLMessengerHistoryManager2);


class __declspec(novtable) IUOLMessengerHistoryManager3: public IUOLMessengerHistoryManager2
{
public:
	virtual ~IUOLMessengerHistoryManager3() {}
	__declspec(deprecated) virtual BOOL ReadLogFilesFromFolder2(CAtlMap<int, CString>& mapLog, IUOLMessengerHistoryContactPtr pContact, const CString& strParameter, const CString& strCallStatus) = 0;
	__declspec(deprecated) virtual BOOL SetConversationWindowClosed2(IUOLMessengerHistoryContactPtr pContact, CTime startTime) = 0;
	__declspec(deprecated) virtual BOOL SetLog(IUOLMessengerHistoryContactPtr pContact, IUOLMessengerXMLManagerPtr pXMLManager, CTime startTime, const CString& strType) = 0;
	__declspec(deprecated) virtual IUOLMessengerHistoryContactPtr GetHistoryContact(const CString& strName, const CString& strAlias, const CString& strProtocol, const CString& strAccountName) = 0;
	__declspec(deprecated) virtual IUOLMessengerXMLManagerPtr GetXMLManager(const CString& strFrom, const CString& strTime,const CString& strStatus, const CString& strContact, const CString& strId, const CString& strDate, const CString& strProtocol, const CString& strType, const CString& strMessage) = 0;
};
MAKEAUTOPTR(IUOLMessengerHistoryManager3);


class __declspec(novtable) IUOLMessengerHistoryManager4: public IUOLMessengerHistoryManager3
{
public:
	virtual ~IUOLMessengerHistoryManager4() {}
	virtual void LogEvent(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
	virtual void GetEventsList(IUOLMessengerHistoryRecordPtr pHistoryRecord, CAtlList<CTime>& listHistoryEvents) = 0;
	virtual void GetEventRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
	virtual void GetRecentEventRecord(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
	virtual void DeleteHistory(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
	virtual void DeleteAllHistoryFiles(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
	virtual void DeleteHistoryEvents(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
};
MAKEAUTOPTR(IUOLMessengerHistoryManager4);