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
#include "CryptoProtocolController.h"


CCryptoProtocolController::~CCryptoProtocolController()
{
}


CCryptoProtocolController* CCryptoProtocolController::GetInstance()
{
	static CCryptoProtocolController cryptoProtocolController;
	
	return &cryptoProtocolController;
}


BOOL CCryptoProtocolController::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	if (CreateProtocols(pUOLMessengerFacade))
	{
		m_pUOLMessengerFacade = pUOLMessengerFacade;

		return TRUE;
	}

	m_pUOLMessengerFacade = NULL;
	return FALSE;
}


void CCryptoProtocolController::Finalize()
{
	if (m_pUOLMessengerFacade)
	{
		DestroyProtocols(m_pUOLMessengerFacade);
	}
}


CCryptoProtocol* CCryptoProtocolController::GetProtocol(const CString& strProtocolId)
{
	CCryptoProtocol* pProtocol = NULL;

	CCryptoProtocolMap::CPair* pPair = m_mapProtocols.Lookup(strProtocolId);

	if (pPair)
	{
		pProtocol = pPair->m_value;
	}

	return pProtocol;
}


CCryptoProtocolController::CCryptoProtocolController() : 
	m_pUOLMessengerFacade(NULL)
{
}


BOOL CCryptoProtocolController::CreateProtocols(IUOLMessengerFacade* pUOLMessengerFacade)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	IUOLMessengerProtocolManagerPtr pProtocolManager = pUOLMessengerFacade->GetProtocolManager();

	if (NULL == pProtocolManager)
	{
		return FALSE;
	}

	CString strNonUIMWarning; 
	IUOLMessengerFacade2* pFacade2 = static_cast<IUOLMessengerFacade2*>(pUOLMessengerFacade);
	
	if (pFacade2)
	{
		IUOLMessengerCommonSettingsPtr pCommonSettings = 
				pFacade2->GetCommonSettings();
		
		if (pCommonSettings.get())
		{
			if (0 == (pCommonSettings->GetAppMode()).Compare(IUOLMessengerCommonSettings::APP_MODE_DEFAULT_VALUE) )
			{
				strNonUIMWarning.LoadString(IDS_NON_UIM_WARNING);
			}
			else
			{
				strNonUIMWarning.LoadString(IDS_NON_UOLVOIP_WARNING);
			}
		}
	}

	m_listProtocols.RemoveAll();

	CIMProtocol* pProtocol = NULL;
	CConstAtlListIterator<CIMProtocol*> itProtocolsList;
	pProtocolManager->GetProtocolList(itProtocolsList);

	while (itProtocolsList != NULL)
	{
		pProtocol = *itProtocolsList;

		m_listProtocols.AddTail(CString((pProtocol->GetId()).c_str()));

		itProtocolsList++;
	}

	for (POSITION pos = m_listProtocols.GetHeadPosition(); pos; )
	{
		CString strProtocolId = m_listProtocols.GetNext(pos);

		CCryptoEventsReceiver* pCryptoEventsReceiver = new CCryptoEventsReceiver(strProtocolId);

		CCryptoProtocol* pCryptoProtocol = pCryptoProtocol = new CCryptoProtocol(pCryptoEventsReceiver, 
																				 std::string(static_cast<LPCTSTR>(strNonUIMWarning)));

		m_listCryptoEventsReceivers.AddTail(pCryptoEventsReceiver);

		pProtocolManager->AddProtocolToStack(strProtocolId, pCryptoProtocol, pCryptoProtocol);

		m_mapProtocols.SetAt(strProtocolId, pCryptoProtocol);
	}

	return TRUE;
}


void CCryptoProtocolController::DestroyProtocols(IUOLMessengerFacade* pUOLMessengerFacade)
{
	if (!m_pUOLMessengerFacade)
	{
		return;
	}

	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	IUOLMessengerProtocolManagerPtr pProtocolManager = pUOLMessengerFacade->GetProtocolManager();

	if (NULL == pProtocolManager)
	{
		ATLASSERT(FALSE);
		return;
	}

	CCryptoProtocolMap::CPair* pPair;

	for (POSITION pos = m_listProtocols.GetHeadPosition(); pos; )
	{
		CString strProtocolId = m_listProtocols.GetNext(pos);

		pPair = m_mapProtocols.Lookup(strProtocolId);

		if (pPair != NULL)
		{
			CCryptoProtocol* pCryptoProtocol = pPair->m_value;

			pProtocolManager->RemoveProtocolFromStack(strProtocolId, pCryptoProtocol, pCryptoProtocol);

			m_mapProtocols.RemoveKey(strProtocolId);
			delete pCryptoProtocol;
		}
	}

	m_mapProtocols.RemoveAll();
	m_listProtocols.RemoveAll();

	for (POSITION pos = m_listCryptoEventsReceivers.GetHeadPosition(); pos;)
	{
		CCryptoEventsReceiver* pCrypoEventsReceiver = m_listCryptoEventsReceivers.GetNext(pos);
		delete pCrypoEventsReceiver;
	}

	m_listCryptoEventsReceivers.RemoveAll();
}