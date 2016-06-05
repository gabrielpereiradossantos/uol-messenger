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


#include "StdAfx.h"
#include ".\cryptoprotocol.h"

using namespace CryptoPP;
using namespace std;



CCryptoProtocol::CCryptoProtocol(ICryptoEvents* pCryptoEvents, 
								 const std::string& strNonUIMWarning) : 
	m_rng(),
	m_mapSession(),
	m_pCryptoEvents(pCryptoEvents),
	m_strNonUIMWarning(strNonUIMWarning)
{
}

CCryptoProtocol::~CCryptoProtocol(void)
{
	CSessionMap::iterator itSession;
	for (itSession = m_mapSession.begin(); itSession != m_mapSession.end(); itSession++)
	{
		delete itSession->second;
	}
}

bool CCryptoProtocol::StartCryptoSession(const std::string& strFromUser, const std::string& strToUser)
{
	ATLTRACE("%s\n", __FUNCTION__);
	
	m_pCryptoEvents->OnCryptoSessionBegin(strFromUser, strToUser);

	StartAcceptResponseTimeoutTimer(strFromUser, strToUser);

	CCryptoSession* pCryptoSession = GetSession(strFromUser, strToUser, true);	

	string strMsg = pCryptoSession->MakeInvite(strFromUser);	

	bool bRet = FALSE; 
		
	CIMProtocol* pNext = GetNextProtocol();
	if (pNext)
	{
		bRet = (IM_PROTOCOL_ERROR_SUCCESS == pNext->SendIM(strFromUser, strToUser, strMsg));
	}

	if (FALSE == bRet)
	{
		EndCryptoSession(strFromUser, strToUser, true);
	}

	return bRet;
}

bool CCryptoProtocol::EndCryptoSession(const std::string& strFromUser, const std::string& strToUser, bool bError)
{
	CCryptoSession* pCryptoSession = GetSession(strFromUser, strToUser) ;

	if (pCryptoSession)
	{
		CIMProtocol* pNext = GetNextProtocol();
		if (pNext)
		{
			pNext->SendIM(strFromUser, strToUser, pCryptoSession->MakeEnd());
		}

		return EndCryptoSession(pCryptoSession, strFromUser, strToUser, bError ? CCryptoSession::reasonCryptoLib : CCryptoSession::reasonNoError);
	}

	return true;
}

CCryptoSession* CCryptoProtocol::GetSession(const std::string& strUser, const std::string& strContact)
{
	return GetSession(strUser, strContact, false);
}


unsigned CCryptoProtocol::GetProtocolPos()
{
	return UINT_MAX - 0xff;	
}

unsigned CCryptoProtocol::GetCallbackPos()
{
	return GetProtocolPos();
}

void CCryptoProtocol::Init()
{
	m_wndSessionTimer.Init();
}

void CCryptoProtocol::Finalize()
{
	m_wndSessionTimer.Finalize();
}

std::string CCryptoProtocol::GetId()
{
	CIMProtocol* pNext = GetNextProtocol();
	if (pNext)
	{
		return pNext->GetId();
	}

	return "prpl-uolcrypto";
}

std::string CCryptoProtocol::GetName()
{
	CIMProtocol* pNext = GetNextProtocol();
	if (pNext)
	{
		return pNext->GetName();
	}

	return "Criptografia UOL";
}


IMProtocolError CCryptoProtocol::SendIM(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage)
{
	string strMsg = strMessage;

	CCryptoSession* pCryptoSession = GetSession(strFromUser, strToUser);

	if (pCryptoSession)
	{
		CCryptoSession::State state = pCryptoSession->GetCurrentState();

		if ((state == CCryptoSession::enSetKey) ||
			(state == CCryptoSession::enAccept) ||
			(state == CCryptoSession::enMsg))
		{
			IMProtocolError sendError = IM_PROTOCOL_ERROR_FAILED;

			CIMProtocol* pNext = GetNextProtocol();

			if (pNext)
			{
				sendError = pNext->SendIM(strFromUser, strToUser, pCryptoSession->MakeMsg(strMessage));
			}

			return sendError;
		}		
	}	

	return CIMProtocol::SendIM(strFromUser, strToUser, strMsg);
}


void CCryptoProtocol::OnIMReceived(const std::string& strToUser, const std::string& strFromUser, const std::string& strText)
{
	m_wndSessionTimer.SynchronizeIMReceived(strToUser, strFromUser, strText, this);
}

void CCryptoProtocol::OnIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	m_wndSessionTimer.SynchronizeIMSent(strFromUser, strToUser, strText, this);
}

void CCryptoProtocol::OnTimer(const std::string& strUser, const std::string& strContact)
{
	CancelAcceptResponseTimeoutTimer(strUser, strContact);

	RemoveSession(strUser, strContact);

	m_pCryptoEvents->OnError(strUser, strContact, CCryptoSession::reasonAcceptTimeout);

	//CIMProtocolCallback* pPrev = GetPreviousCallback();

	//if (pPrev)
	//{
	//	pPrev->OnIMReceived(strUser, strContact, m_strCantStartCryptoSession);
	//}
}

void CCryptoProtocol::OnSyncIMReceived(const std::string& strToUser, 
		const std::string& strFromUser, const std::string& strText)
{
	string strMsg = strText;
	bool bSendMessageToPrev = true;

	if (CCryptoSession::IsCryptoMsg(strMsg))
	{
		CIMProtocol* pNext = GetNextProtocol();

		bSendMessageToPrev = false;
		CCryptoSession* pSession = NULL;
		
		if (CCryptoSession::IsInviteMsg(strMsg))
		{
			pSession = GetSession(strToUser, strFromUser, true);
		}
		else
		{
			pSession = GetSession(strToUser, strFromUser, false);
		}

		if (pSession)
		{
			CCryptoSession::State stateOld = pSession->GetCurrentState();

			string strProcessed = pSession->ProcessCryptoMsg(strFromUser, strToUser, strMsg);

			if (pSession->GetCurrentState() == CCryptoSession::enInvite)
			{
				if ((stateOld != CCryptoSession::enInit) &&
					(stateOld != CCryptoSession::enEnd))
				{
					ATLTRACE("%s (1) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					m_pCryptoEvents->OnCryptoSessionEnd(strToUser, strFromUser);

					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeEnd());
					}
				}
				else
				{
					ATLTRACE("%s (2) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					m_pCryptoEvents->OnInvite(strToUser, strFromUser);

					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeAccept());
					}
				}
			}
			else if (pSession->GetCurrentState() == CCryptoSession::enAccept)
			{
				if ((stateOld != CCryptoSession::enInit) &&
					(stateOld != CCryptoSession::enEnd))
				{
					ATLTRACE("%s (3) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					m_pCryptoEvents->OnCryptoSessionEnd(strToUser, strFromUser);

					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeEnd());
					}
				}
				else
				{
					ATLTRACE("%s (4) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					CancelAcceptResponseTimeoutTimer(strToUser, strFromUser);

					m_pCryptoEvents->OnAccept(strToUser, strFromUser);

					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeSetKey(strFromUser, strToUser));			
					}
				}
			}
			else if (pSession->GetCurrentState() == CCryptoSession::enSetKey)
			{
				if (stateOld != CCryptoSession::enInvite)
				{
					ATLTRACE("%s (5) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					m_pCryptoEvents->OnCryptoSessionEnd(strToUser, strFromUser);

					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeEnd());
					}
				}
				else
				{
					m_pCryptoEvents->OnSetKey(strToUser, strFromUser);
				}
			}
			else if (pSession->GetCurrentState() == CCryptoSession::enMsg)
			{
				if ((stateOld != CCryptoSession::enAccept) &&
					(stateOld != CCryptoSession::enSetKey) && 
					(stateOld != CCryptoSession::enMsg))
				{
					ATLTRACE("%s (6) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					m_pCryptoEvents->OnCryptoSessionEnd(strToUser, strFromUser);

					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeEnd());
					}
				}
				else
				{
					m_pCryptoEvents->OnMsg(strToUser, strFromUser);

					strMsg = strProcessed;					

					bSendMessageToPrev = true;	
				}
			}		
			else if (pSession->GetCurrentState() == CCryptoSession::enEnd)
			{
				ATLTRACE("%s (7) -> GetCurrentState() == %d,  and stateOld = %d\n", 
						__FUNCTION__, pSession->GetCurrentState(), stateOld);
				
				EndCryptoSession(pSession, strToUser, strFromUser, CCryptoSession::reasonNoError);
			}
			else //if (pSession->GetCurrentState() == CCryptoSession::enError)
			{
				if (stateOld != CCryptoSession::enMsg)
				{
					ATLTRACE("%s (8) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					// Session not found
					if (pNext)
					{
						pNext->SendIM(strToUser, strFromUser, pSession->MakeError(CCryptoSession::reasonSessionNotFound));
					}

					EndCryptoSession(pSession, strToUser, strFromUser, CCryptoSession::reasonCryptoLib);
				}
				else
				{
					ATLTRACE("%s (9) -> GetCurrentState() == %d,  and stateOld = %d\n", 
							__FUNCTION__, pSession->GetCurrentState(), stateOld);
					
					EndCryptoSession(pSession, strToUser, strFromUser, CCryptoSession::reasonSessionNotFound);

					// Restart session
					//StartCryptoSession(strToUser, strFromUser);
				}
			}
		}
	}

	if (bSendMessageToPrev)
	{
		CIMProtocolCallback* pPrev = GetPreviousCallback();

		if (pPrev)
		{
			pPrev->OnIMReceived(strToUser, strFromUser, strMsg);
		}
	}
}

void CCryptoProtocol::OnSyncIMSent(const std::string& strFromUser, 
		const std::string& strToUser, const std::string& strText)
{
	string strMsg = strText;

	CIMProtocolCallback* pPrev = NULL;

	if (CCryptoSession::IsCryptoMsg(strMsg))
	{
		CCryptoSession* pSession = GetSession(strFromUser, strToUser);

		if (pSession)
		{
			CCryptoSession::State state = pSession->GetCurrentState();

			if ((state == CCryptoSession::enMsg) ||
				(state == CCryptoSession::enAccept) ||
				(state == CCryptoSession::enSetKey))
			{
				string strProcessed = pSession->ProcessCryptoMsg(strFromUser, strToUser, strMsg);

				if (strProcessed.size())
				{
					pPrev = GetPreviousCallback();

					if (pPrev)
					{
						pPrev->OnIMSent(strFromUser, strToUser, strProcessed);
					}
				}
			}
		}
	}
	else
	{
		// Repass non-encrypted messages to previous callback...
		pPrev = GetPreviousCallback();

		if (pPrev)
		{
			pPrev->OnIMSent(strFromUser, strToUser, strText);
		}
	}
}


CCryptoSession* CCryptoProtocol::GetSession(const std::string& strUser, const std::string& strContact, bool bCreate)
{
	string strSessionId = CCryptoSession::BuildSessionId(strUser, strContact);

	CSessionMap::iterator it = m_mapSession.find(strSessionId);

	CCryptoSession* pCryptoSession = NULL;

	if (it == m_mapSession.end())
	{
		if (bCreate)
		{
			pCryptoSession = new CCryptoSession(m_mapPubKeys, m_rng, m_strNonUIMWarning.c_str());

			m_mapSession[strSessionId] = pCryptoSession;
		}
	}
	else
	{
		pCryptoSession = it->second;
	}	
	
	return pCryptoSession;
}

void CCryptoProtocol::RemoveSession(const std::string& strUser, const std::string& strContact)
{
	string strSessionId = CCryptoSession::BuildSessionId(strUser, strContact);

	CSessionMap::iterator it = m_mapSession.find(strSessionId);

	if (it != m_mapSession.end())
	{
		delete it->second;

		m_mapSession.erase(it);
	}

	m_mapPubKeys.erase(strSessionId);
}

void CCryptoProtocol::StartAcceptResponseTimeoutTimer(const std::string& strFromUser, const std::string& strToUser)
{
	m_wndSessionTimer.StartTimer(strFromUser, strToUser, this);
}

void CCryptoProtocol::CancelAcceptResponseTimeoutTimer(const std::string& strUser, const std::string& strContact)
{
	m_wndSessionTimer.StopTimer(strUser, strContact);
}


bool CCryptoProtocol::EndCryptoSession(CCryptoSession* pSession, 
									   const string& strFromUser, 
									   const string& strToUser, 
									   CCryptoSession::ErrorReason reason)
{
	ATLTRACE("%s -> reason = %d\n", __FUNCTION__, reason);
	
	RemoveSession(strFromUser, strToUser);	

	if (reason == CCryptoSession::reasonNoError)
	{
		m_pCryptoEvents->OnCryptoSessionEnd(strFromUser, strToUser);
	}
	else
	{
		m_pCryptoEvents->OnError(strFromUser, strToUser, reason);
	}

	return GetSession(strFromUser, strToUser) == FALSE;
}