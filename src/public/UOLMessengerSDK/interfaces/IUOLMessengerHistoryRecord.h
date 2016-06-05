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

#define	HR_VERSION	_T("1.0")
#define HR_RECENT_HISTORY_MAX_EVENTS 10

///////////////////////////////////////////////////////////////////////////////
// IUOLMessengerHistoryEvent Interface
///////////////////////////////////////////////////////////////////////////////
class __declspec(novtable) IUOLMessengerHistoryEvent
{
public:
	virtual ~IUOLMessengerHistoryEvent() {}

	virtual CString GetDate() const = 0;
	virtual CString GetTime() const = 0;
	virtual CTime GetFullTime() const = 0;
	virtual CString GetStatus() const = 0;
	virtual CString GetType() const = 0;

	virtual CString GetEventFrom() const = 0;
	virtual CString GetEventTo() const = 0;
	virtual CString GetEventText() const = 0;
	virtual CString GetEventId() const = 0;
};
MAKEAUTOPTR(IUOLMessengerHistoryEvent);


///////////////////////////////////////////////////////////////////////////////
// IUOLMessengerHistoryRecord Interface
///////////////////////////////////////////////////////////////////////////////
class IUOLMessengerHistoryRecord;
MAKEAUTOPTR(IUOLMessengerHistoryRecord);
class __declspec(novtable) IUOLMessengerHistoryRecord
{
public:
	virtual ~IUOLMessengerHistoryRecord() {}
	
	virtual CString GetVersion() const = 0;
	virtual CString GetService() const = 0;
	virtual void SetService(const CString& strService) = 0;
	virtual CString GetAccountUserId() const = 0;
	virtual CString GetContactName() const = 0;
	virtual CString GetProtocolId() const = 0;

	virtual void GetEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents) = 0;
	virtual void GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents) = 0;
	virtual void AddEvent(IUOLMessengerHistoryEventPtr pHistoryEvent) = 0;
	virtual void RemoveEvent(IUOLMessengerHistoryEventPtr pHistoryEvent) = 0;
	virtual void ClearEvents() = 0;

	virtual BOOL CanLog() const = 0;
	virtual CString	GetHistoryPath() const = 0;
	virtual CString	GetFileName() const = 0;
	virtual CTime GetTime() const = 0;
	virtual void SetTime(const CTime& timeHistory) = 0;

	virtual BOOL ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord) = 0;
	virtual CString GetHistoryText() = 0;
	virtual CString GetRecentHistoryText() = 0;
};


///////////////////////////////////////////////////////////////////////////////
// CUOLMessengerHistoryRecordListObserver Interface
///////////////////////////////////////////////////////////////////////////////
class IUOLMessengerHistoryRecordList;
MAKEAUTOPTR(IUOLMessengerHistoryRecordList);
class CUOLMessengerHistoryRecordListObserver
{
public:
	virtual ~CUOLMessengerHistoryRecordListObserver() {}

	virtual void OnUpdateHistoryRecordListStart(IUOLMessengerHistoryRecordPtr pHistoryRecord) {}
	virtual void OnUpdateHistoryRecordListFinish() {}
};


///////////////////////////////////////////////////////////////////////////////
// IUOLMessengerHistoryRecordList Interface
///////////////////////////////////////////////////////////////////////////////
class IUOLMessengerHistoryRecordList
{
public:
	virtual ~IUOLMessengerHistoryRecordList() {};

	virtual CString GetHistoryText(CTime eventTime) = 0;
	virtual IUOLMessengerHistoryRecordPtr GetHistoryRecord(CTime eventTime) = 0;
	virtual IUOLMessengerHistoryRecordPtr GetHistoryRecordByIndex(int nIndex) = 0;
	virtual int GetCount() = 0;
	virtual void GetHistoryEvents(CAtlList<CTime>& listHistoryEvents) = 0;
	virtual void DeleteHistory(CTime eventTime) = 0;
	virtual void DeleteAllHistory() = 0;
	virtual void RemoveAll() = 0;
	virtual CString GetProtocolId() = 0;
	virtual BOOL GetShowTime() = 0;
	virtual BOOL StartLoad() = 0;
	virtual BOOL StopLoad() = 0;

	virtual void RegisterObserver(CUOLMessengerHistoryRecordListObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerHistoryRecordListObserver* pObserver) = 0;
};
MAKEAUTOPTR(IUOLMessengerHistoryRecordList);