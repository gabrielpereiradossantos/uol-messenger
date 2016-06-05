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

#include <interfaces/IUOLMessengerHistoryRecord.h>
#include "../ObserverRegister.h"


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordListNotifier
///////////////////////////////////////////////////////////////////////////////
class CHistoryRecordListNotifier : public CObserverRegister<CUOLMessengerHistoryRecordListObserver>
{
public:
	void NotifyAll(void (CUOLMessengerHistoryRecordListObserver::*pfnUpdate)(IUOLMessengerHistoryRecordPtr), IUOLMessengerHistoryRecordPtr pHistoryRecord);
	void NotifyAll(void (CUOLMessengerHistoryRecordListObserver::*pfnUpdate)());
};


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordEvent
///////////////////////////////////////////////////////////////////////////////
class CHistoryRecordEvent : public IUOLMessengerHistoryEvent
{
public:	
	CHistoryRecordEvent(const CTime& timeEvent, const CString& strStatus, 
		const CString& strType, const CString& strEventFrom,
		const CString& strEventTo, const CString& strEventText);
	virtual ~CHistoryRecordEvent();
	
	virtual CString GetDate() const;
	virtual CString GetTime() const;
	virtual CString GetStatus() const;
	virtual CString GetType() const;
	virtual CString GetEventFrom() const;
	virtual CString GetEventTo() const;
	virtual CString GetEventText() const;
	virtual CString GetEventId() const;
	virtual CTime GetFullTime() const;

private:
	CTime		m_timeEvent;
	CString		m_strStatus;
	CString		m_strType;
	CString		m_strEventFrom;
	CString		m_strEventTo;
	CString		m_strEventText;
};

MAKEAUTOPTR(CHistoryRecordEvent);

///////////////////////////////////////////////////////////////////////////////
// CHistoryRecord
///////////////////////////////////////////////////////////////////////////////
class CHistoryRecord : public IUOLMessengerHistoryRecord
{
public:
	CHistoryRecord();
	virtual ~CHistoryRecord();

	// IUOLMessengerHistoryRecord interface
	virtual CString GetVersion() const;
	virtual CString GetService() const;
	virtual void SetService(const CString& strService);
	virtual CString GetAccountUserId() const;
	virtual CString GetContactName() const;
	virtual CString GetProtocolId() const;
	
	virtual void GetEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents);
	virtual void AddEvent(IUOLMessengerHistoryEventPtr pHistoryEvent);
	virtual void RemoveEvent(IUOLMessengerHistoryEventPtr pHistoryEvent);
	virtual void ClearEvents();

	virtual BOOL CanLog() const;
	virtual CString	GetHistoryPath() const;
	virtual CString	GetFileName() const;
	virtual CTime GetTime() const;
	virtual void SetTime(const CTime& timeHistory);

	virtual BOOL ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord);
	virtual CString GetHistoryText();
	virtual CString GetRecentHistoryText();

	void GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents);

	void SetVersion(const CString& strVersion);
	void SetAccountUserId(const CString& strAccountUserId);
	void SetProtocolId(const CString& strProtocolId);

protected:
	CString GetType() const;
	CString GetEventFrom() const;
	CString GetEventTo() const;

private:
	CTime		m_timeHistory;
	CString		m_strVersion;
	CString		m_strService;
	CString		m_strAccountUserId;
	CString		m_strProtocolId;

	CAtlList<CHistoryRecordEventPtr>	m_listEvents;
	CComAutoCriticalSection				m_csList;
};
MAKEAUTOPTR(CHistoryRecord);


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordList Class
///////////////////////////////////////////////////////////////////////////////
class CHistoryRecordList : public IUOLMessengerHistoryRecordList
{
public:
	CHistoryRecordList();
	virtual ~CHistoryRecordList();

	virtual CString GetHistoryText(CTime eventTime);
	virtual IUOLMessengerHistoryRecordPtr GetHistoryRecord(CTime eventTime);
	virtual IUOLMessengerHistoryRecordPtr GetHistoryRecordByIndex(int nIndex);
	virtual int GetCount();
	virtual void GetHistoryEvents(CAtlList<CTime>& listHistoryEvents) {};
	virtual void DeleteHistory(CTime eventTime) {};
	virtual void DeleteAllHistory() {};
	virtual void RemoveAll() {};
	virtual CString GetProtocolId();
	virtual BOOL GetShowTime();
	virtual BOOL StartLoad();
	virtual BOOL StopLoad();

	virtual void RegisterObserver(CUOLMessengerHistoryRecordListObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerHistoryRecordListObserver* pObserver);

protected:
	CHistoryRecordListNotifier m_ObserverNotifier;
};
MAKEAUTOPTR(CHistoryRecordList);