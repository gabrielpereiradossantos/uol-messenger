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
#include "SettingsManagerObserverNotifier.h"


void CSettingsManagerObserverNotifier::RegisterObserver(CUOLMessengerSettingsManagerObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
		{
			CSettingsManagerObserverWrapperPtr pObserverWrapper = m_listObservers.GetNext(pos);

			if (pObserver == pObserverWrapper->GetWrappedObserver())
			{
				// Observer already registered
				ATLASSERT(FALSE);
				return;
			}
		}

		m_listObservers.AddTail(new CSettingsManagerObserverWrapper(pObserver));
	}
}


void CSettingsManagerObserverNotifier::UnregisterObserver(CUOLMessengerSettingsManagerObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CSettingsManagerObserverWrapperPtr pRemovedObserverWrapper;
		
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

			for (POSITION pos = m_listObservers.GetHeadPosition(); pos; m_listObservers.GetNext(pos))
			{
				CSettingsManagerObserverWrapperPtr pObserverWrapper = m_listObservers.GetAt(pos);

				if (pObserver == pObserverWrapper->GetWrappedObserver())
				{
					// Observer found
					pRemovedObserverWrapper = pObserverWrapper;
					m_listObservers.RemoveAt(pos);
					break;
				}
			}
		}

		//ATLASSERT(pRemovedObserverWrapper);
		if (pRemovedObserverWrapper)
		{
			pRemovedObserverWrapper->DisableNotifications();
		}
	}
}


void CSettingsManagerObserverNotifier::UnregisterAll()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; m_listObservers.GetNext(pos))
	{
		CSettingsManagerObserverWrapperPtr pObserverWrapper = m_listObservers.GetAt(pos);

		pObserverWrapper->DisableNotifications();
	}

	m_listObservers.RemoveAll();
}


void CSettingsManagerObserverNotifier::NotifyAll(void (CUOLMessengerSettingsManagerObserver::*pfnCallback)())
{
	CAtlList<CSettingsManagerObserverWrapperPtr> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	for (POSITION pos = listObservers.GetHeadPosition(); pos; )
	{
		CSettingsManagerObserverWrapperPtr pObserver = listObservers.GetNext(pos);

		((pObserver.get())->*pfnCallback)();
	}
}

