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

#include <util/Thread.h>
#include <util/IRunnable.h>
#include <interfaces/IUOLMessengerHistoryRecord.h>
#include <interfaces/IUOLMessengerConversation.h>
#include "HistoryRecord.h"


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordMessage
///////////////////////////////////////////////////////////////////////////////
class CHistoryRecordMessage : public CHistoryRecord
{
public:
	// this constructor must be used when writing to xml
	CHistoryRecordMessage(IUOLMessengerAccountPtr pAccount, const CString& strContact,
		IUOLMessengerConversation::MessageEntryPtr pMessageEntry);

	// this constructor must be used when reading from xml
	CHistoryRecordMessage(IUOLMessengerAccountPtr pAccount, const CString& strContact);

	virtual ~CHistoryRecordMessage();

	// IUOLMessengerHistoryRecord interface
	virtual BOOL CanLog() const;
	virtual BOOL ConvertFrom(IUOLMessengerHistoryRecordPtr pHistoryRecord);
	virtual CString GetHistoryText();
	virtual CString GetRecentHistoryText();
	virtual void GetRecentEvents(CAtlList<IUOLMessengerHistoryEventPtr>& listEvents);
	virtual CString GetContactName() const;
	
	IUOLMessengerAccountPtr GetAccount();
	
protected:
	CString GetType() const;
	CString GetEventFrom() const;
	CString GetEventTo() const;
	CString GetPrplProtocolId() const;
	CString FindAlias(const CString& strName);

private:
	IUOLMessengerAccountPtr						m_pAccount;
	IUOLMessengerConversation::MessageEntryPtr	m_pMessageEntry;
	
	CString		m_strContact;
	CString		m_strHistoryText;
	CString		m_strRecentHistoryText;
};
MAKEAUTOPTR(CHistoryRecordMessage);


///////////////////////////////////////////////////////////////////////////////
// CHistoryRecordMessageList Class
///////////////////////////////////////////////////////////////////////////////
class CHistoryRecordMessageList : 
	public IRunnable,
	public CHistoryRecordList
{
public:
	CHistoryRecordMessageList(CHistoryRecordMessagePtr pHistoryRecordMessage);
	virtual ~CHistoryRecordMessageList();

	virtual CString GetHistoryText(CTime eventTime);
	virtual IUOLMessengerHistoryRecordPtr GetHistoryRecord(CTime eventTime);
	virtual IUOLMessengerHistoryRecordPtr GetHistoryRecordByIndex(int nIndex);
	virtual int GetCount();
	virtual void GetHistoryEvents(CAtlList<CTime>& listHistoryEvents);
	virtual void DeleteHistory(CTime eventTime);
	virtual void DeleteAllHistory();
	virtual void RemoveAll();
	virtual CString GetProtocolId();
	virtual BOOL StartLoad();
	virtual BOOL StopLoad();
	
protected:
	void GetEventRecord(CHistoryRecordMessagePtr pHistoryRecordMessage);
	void Run();

private:
	CAtlList<CHistoryRecordMessagePtr>	m_listHistoryRecordMessage;
	CHistoryRecordMessagePtr			m_pHistoryRecordMessage;
	CComAutoCriticalSection				m_csList;
	
	CThread		m_thread;
	BOOL		m_bStopFlag;
};
MAKEAUTOPTR(CHistoryRecordMessageList);