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

#include "cryptoSession.h"
#include "SessionTimerWindow.h"

typedef std::map<std::string, CCryptoSession*> CSessionMap;
typedef std::map<std::string, std::string>	   CPubKeyMap;



class ICryptoEvents
{
public:
	virtual ~ICryptoEvents() {}

	virtual void OnCryptoSessionBegin(const std::string& strFromUser, const std::string& strToUser) = 0;
	virtual void OnCryptoSessionEnd(const std::string& strFromUser, const std::string& strToUser) = 0;
	virtual void OnInvite(const std::string& strFromUser, const std::string& strToUser) = 0;
	virtual void OnAccept(const std::string& strFromUser, const std::string& strToUser) = 0;
	virtual void OnSetKey(const std::string& strFromUser, const std::string& strToUser) = 0;
	virtual void OnMsg(const std::string& strFromUser, const std::string& strToUser) = 0;		
	virtual void OnError(const std::string& strFromUser, const std::string& strToUser, CCryptoSession::ErrorReason reason) = 0;
};


class CCryptoProtocol : public CIMProtocol7, 
						public CIMProtocolCallback7,
						public ITimerWindowCallback,
						public ISynchronizeIMCallback
{
public:
	CCryptoProtocol(ICryptoEvents* pCryptoEvents,
					const std::string& strNonUIMWarning);
	virtual ~CCryptoProtocol(void);

	bool StartCryptoSession(const std::string& strFromUser, const std::string& strToUser);
	bool EndCryptoSession(const std::string& strFromUser, 
						  const std::string& strToUser, 
						  bool bError);

	CCryptoSession* GetSession(const std::string& strUser, const std::string& strContact);
	
protected:
	virtual unsigned GetProtocolPos();
	virtual unsigned GetCallbackPos();

	virtual void Init();
	virtual void Finalize();

	virtual std::string GetId();
	virtual std::string GetName();

	// IM
	virtual IMProtocolError SendIM(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage);

	// CIMProtocolCallback
	virtual void OnIMReceived(const std::string& strToUser, const std::string& strFromUser, const std::string& strText);
	virtual void OnIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText);

	virtual void OnTimer(const std::string& strUser, const std::string& strContact);
	
	// ISynchronizeIMCallback.
	virtual void OnSyncIMReceived(const std::string& strToUser, const std::string& strFromUser, const std::string& strText);
	virtual void OnSyncIMSent(const std::string& strFromUser, const std::string& strToUser, const std::string& strText);

private:
	CCryptoSession* GetSession(const std::string& strUser, const std::string& strContact, bool bCreate);
	void RemoveSession(const std::string& strUser, const std::string& strContact);
	void StartAcceptResponseTimeoutTimer(const std::string& strUser, const std::string& strContact);
	void CancelAcceptResponseTimeoutTimer(const std::string& strUser, const std::string& strContact);

	bool EndCryptoSession(CCryptoSession* pSession, 
						  const std::string& strFromUser, 
						  const std::string& strToUser, 
						  CCryptoSession::ErrorReason reason);

private:
	CryptoPP::AutoSeededX917RNG<CryptoPP::DES_EDE3> m_rng;

	CSessionMap										m_mapSession;	
	CPubKeyMap										m_mapPubKeys;

	ICryptoEvents*									m_pCryptoEvents;
	std::string										m_strNonUIMWarning;
	std::string										m_strCantStartCryptoSession;

	CSessionTimerWindow								m_wndSessionTimer;
};
