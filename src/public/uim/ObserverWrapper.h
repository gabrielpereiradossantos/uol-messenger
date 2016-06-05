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


#define BEGIN_HANDLER_CALL() \
	if (FALSE == m_bNotificationEnabled) \
	{ \
		return; \
	} \
	::InterlockedIncrement(&m_nNotificationCount); \

#define END_HANDLER_CALL() \
	::InterlockedDecrement(&m_nNotificationCount); \



template <typename TObserver>
class CObserverWrapper : public TObserver
{
public:
	CObserverWrapper(TObserver* pWrappedObserver) :
	  	m_bNotificationEnabled(TRUE),
		m_nNotificationCount(0)
	{
		m_pObserver = pWrappedObserver;
	}

	virtual ~CObserverWrapper() {}

	TObserver* GetWrappedObserver()
	{
		return m_pObserver;
	}

	void DisableNotifications()
	{
		::InterlockedExchange(&m_bNotificationEnabled, FALSE);

		while (m_nNotificationCount > 0)
		{
			::Sleep(1);
		}
	}

	// HandleNotification
	//void HandleNotification()
	//{
	//	BEGIN_HANDLER_CALL();

	//	if (m_pObserver)
	//	{
	//		m_pObserver->HandleNotification();
	//	}

	//	END_HANDLER_CALL();
	//}

protected:
	TObserver*	m_pObserver;
	volatile LONG	m_bNotificationEnabled;
	volatile LONG	m_nNotificationCount;
};


template <typename TBaseObserver, typename TDerivedObserver>
class CDerivedObserverWrapper : public TDerivedObserver
{
public:
	CDerivedObserverWrapper(TBaseObserver* pWrappedObserver) :
	  	m_bNotificationEnabled(TRUE),
		m_nNotificationCount(0)
	{
		m_pObserver = pWrappedObserver;
	}

	virtual ~CDerivedObserverWrapper() {}

	TBaseObserver* GetWrappedObserver()
	{
		return m_pObserver;
	}

	TDerivedObserver* GetWrappedDerivedObserver()
	{
		TDerivedObserver* pDerivedObserver = dynamic_cast<TDerivedObserver*>(m_pObserver);
		return pDerivedObserver;
	}

	void DisableNotifications()
	{
		::InterlockedExchange(&m_bNotificationEnabled, FALSE);

		while (m_nNotificationCount > 0)
		{
			::Sleep(1);
		}
	}

	// HandleNotification
	//void HandleNotification()
	//{
	//	BEGIN_HANDLER_CALL();

	//	if (m_pObserver)
	//	{
	//		m_pObserver->HandleNotification();
	//	}

	//	END_HANDLER_CALL();
	//}

protected:
	TBaseObserver*	m_pObserver;
	volatile LONG	m_bNotificationEnabled;
	volatile LONG	m_nNotificationCount;
};